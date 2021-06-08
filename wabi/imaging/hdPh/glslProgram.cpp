/*
 * Copyright 2021 Pixar. All Rights Reserved.
 *
 * Portions of this file are derived from original work by Pixar
 * distributed with Universal Scene Description, a project of the
 * Academy Software Foundation (ASWF). https://www.aswf.io/
 *
 * Licensed under the Apache License, Version 2.0 (the "Apache License")
 * with the following modification; you may not use this file except in
 * compliance with the Apache License and the following modification:
 * Section 6. Trademarks. is deleted and replaced with:
 *
 * 6. Trademarks. This License does not grant permission to use the trade
 *    names, trademarks, service marks, or product names of the Licensor
 *    and its affiliates, except as required to comply with Section 4(c)
 *    of the License and to reproduce the content of the NOTICE file.
 *
 * You may obtain a copy of the Apache License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the Apache License with the above modification is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 * ANY KIND, either express or implied. See the Apache License for the
 * specific language governing permissions and limitations under the
 * Apache License.
 *
 * Modifications copyright (C) 2020-2021 Wabi.
 */
#include "wabi/imaging/hdPh/glslProgram.h"
#include "wabi/base/arch/hash.h"
#include "wabi/base/tf/diagnostic.h"
#include "wabi/imaging/hd/perfLog.h"
#include "wabi/imaging/hd/tokens.h"
#include "wabi/imaging/hdPh/debugCodes.h"
#include "wabi/imaging/hdPh/glUtils.h"
#include "wabi/imaging/hdPh/package.h"
#include "wabi/imaging/hdPh/resourceRegistry.h"
#include "wabi/imaging/hgi/tokens.h"
#include "wabi/imaging/hio/glslfx.h"

#include <climits>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

WABI_NAMESPACE_BEGIN

// Get the line number from the compilation error message, and return a boolean
// indicating success/failure of parsing.
// Note: This has been tested only on nVidia.
static bool _ParseLineNumberOfError(std::string const &error, unsigned int *lineNum)
{
  if (!lineNum) {
    return false;
  }
  // sample error on nVidia:
  // 0(279) : error C1031: swizzle mask element not present in operand "xyz"
  // 279 is the line number here.
  std::string::size_type start = error.find('(');
  std::string::size_type end   = error.find(')');
  if (start != std::string::npos && end != std::string::npos) {
    std::string lineNumStr = error.substr(start + 1, end - 1);
    unsigned long num      = strtoul(lineNumStr.c_str(), nullptr, 10);
    *lineNum               = (unsigned int)num;
    if (num == ULONG_MAX || num == 0) {
      // Out of range, or no valid conversion could be performed.
      return false;
    }
    return true;
  }
  else {
    // Error message isn't formatted as expected.
    return false;
  }
}

// Return the substring for the inclusive range given the start and end indices.
static std::string _GetSubstring(std::string const &str,
                                 std::string::size_type startPos,
                                 std::string::size_type endPos)
{
  if (endPos == std::string::npos) {
    return str.substr(startPos, endPos);
  }
  return str.substr(startPos, endPos - startPos + 1);
}

// It's helpful to have a few more lines around the erroring line when logging
// compiler error messages. This function returns this contextual info
// as a string.
static std::string _GetCompileErrorCodeContext(std::string const &shader,
                                               unsigned int lineNum,
                                               unsigned int contextSize)
{
  unsigned int numLinesToSkip = std::max<unsigned int>(0, lineNum - contextSize - 1);
  std::string::size_type i    = 0;
  for (unsigned int line = 0; line < numLinesToSkip && i != std::string::npos; line++) {
    i = shader.find('\n', i + 1);  // find the next occurrance
  }

  if (i == std::string::npos)
    return std::string();

  // Copy context before the error line.
  std::string::size_type start = i;
  for (unsigned int line = 0; line < contextSize && i != std::string::npos; line++) {
    i = shader.find('\n', i + 1);
  }

  std::string context = _GetSubstring(shader, start, i);

  // Copy error line with annotation.
  start = i + 1;
  i     = shader.find('\n', start);
  context += _GetSubstring(shader, start, i - 1) + " <<< ERROR!\n";

  // Copy context after error line.
  start = i + 1;
  for (unsigned int line = 0; line < contextSize && i != std::string::npos; line++) {
    i = shader.find('\n', i + 1);
  }
  context += _GetSubstring(shader, start, i);

  return context;
}

static void _DumpShaderSource(const char *shaderType, std::string const &shaderSource)
{
  std::cout << "--------- " << shaderType << " ----------\n";
  std::cout << shaderSource;
  std::cout << "---------------------------\n";
  std::cout << std::flush;
}

HdPhGLSLProgram::HdPhGLSLProgram(TfToken const &role, HdPhResourceRegistry *const registry)
    : _registry(registry),
      _role(role)
{
  static size_t globalDebugID = 0;
  _debugID                    = globalDebugID++;
}

HdPhGLSLProgram::~HdPhGLSLProgram()
{
  Hgi *const hgi = _registry->GetHgi();

  if (_program) {
    for (HgiShaderFunctionHandle fn : _program->GetShaderFunctions()) {
      hgi->DestroyShaderFunction(&fn);
    }
    hgi->DestroyShaderProgram(&_program);
  }
}

/* static */
HdPhGLSLProgram::ID HdPhGLSLProgram::ComputeHash(TfToken const &sourceFile)
{
  HD_TRACE_FUNCTION();

  uint32_t hash               = 0;
  std::string const &filename = sourceFile.GetString();
  hash                        = ArchHash(filename.c_str(), filename.size(), hash);

  return hash;
}

bool HdPhGLSLProgram::CompileShader(HgiShaderStage stage, std::string const &shaderSource)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  // early out for empty source.
  // this may not be an error, since glslfx gives empty string
  // for undefined shader stages (i.e. null geometry shader)
  if (shaderSource.empty()) {
    return false;
  }

  const char *shaderType = nullptr;

  switch (stage) {
    case HgiShaderStageVertex:
      shaderType = "GL_VERTEX_SHADER";
      break;
    case HgiShaderStageTessellationControl:
      shaderType = "GL_TESS_CONTROL_SHADER";
      break;
    case HgiShaderStageTessellationEval:
      shaderType = "GL_TESS_EVALUATION_SHADER";
      break;
    case HgiShaderStageGeometry:
      shaderType = "GL_GEOMETRY_SHADER";
      break;
    case HgiShaderStageFragment:
      shaderType = "GL_FRAGMENT_SHADER";
      break;
    case HgiShaderStageCompute:
      shaderType = "GL_COMPUTE_SHADER";
      break;
  }

  if (!shaderType) {
    TF_CODING_ERROR("Invalid shader type %d\n", stage);
    return false;
  }

  if (TfDebug::IsEnabled(HDPH_DUMP_SHADER_SOURCE)) {
    _DumpShaderSource(shaderType, shaderSource);
  }

  Hgi *const hgi = _registry->GetHgi();

  // Create a shader, compile it
  HgiShaderFunctionDesc shaderFnDesc;
  shaderFnDesc.shaderCode          = shaderSource.c_str();
  shaderFnDesc.shaderStage         = stage;
  HgiShaderFunctionHandle shaderFn = hgi->CreateShaderFunction(shaderFnDesc);

  std::string fname;
  if (TfDebug::IsEnabled(HDPH_DUMP_SHADER_SOURCEFILE) ||
      (TfDebug::IsEnabled(HDPH_DUMP_FAILING_SHADER_SOURCEFILE) && !shaderFn->IsValid())) {
    std::stringstream fnameStream;
    static size_t debugShaderID = 0;
    fnameStream << "program" << _debugID << "_shader" << debugShaderID++ << "_" << shaderType
                << ".glsl";
    fname = fnameStream.str();
    std::fstream output(fname.c_str(), std::ios::out);
    output << shaderSource;
    output.close();

    std::cout << "Write " << fname << " (size=" << shaderSource.size() << ")\n";
  }

  if (!shaderFn->IsValid()) {
    std::string logString = shaderFn->GetCompileErrors();
    unsigned int lineNum  = 0;
    if (_ParseLineNumberOfError(logString, &lineNum)) {
      // Get lines surrounding the erroring line for context.
      std::string errorContext = _GetCompileErrorCodeContext(shaderSource, lineNum, 3);
      if (!errorContext.empty()) {
        // erase the \0 if present.
        if (logString.back() == '\0') {
          logString.erase(logString.end() - 1, logString.end());
        }
        logString.append("\nError Context:\n");
        logString.append(errorContext);
      }
    }

    const char *programName = fname.empty() ? shaderType : fname.c_str();
    TF_WARN("Failed to compile shader (%s): %s", programName, logString.c_str());

    if (TfDebug::IsEnabled(HDPH_DUMP_FAILING_SHADER_SOURCE)) {
      _DumpShaderSource(shaderType, shaderSource);
    }

    // shader is no longer needed.
    hgi->DestroyShaderFunction(&shaderFn);

    return false;
  }

  // Store the shader function in the program descriptor so it can be used
  // during Link time.
  _programDesc.shaderFunctions.push_back(shaderFn);

  return true;
}

static const char *_GetShaderType(HgiShaderStage stage)
{
  switch (stage) {
    case HgiShaderStageVertex:
      return "--------GL_VERTEX_SHADER--------\n";
    case HgiShaderStageFragment:
      return "--------GL_FRAGMENT_SHADER--------\n";
    case HgiShaderStageGeometry:
      return "--------GL_GEOMETRY_SHADER--------\n";
    case HgiShaderStageTessellationControl:
      return "--------GL_TESS_CONTROL_SHADER--------\n";
    case HgiShaderStageTessellationEval:
      return "--------GL_TESS_EVALUATION_SHADER--------\n";

    default:
      return "--------UNKNOWN_SHADER_STAGE--------\n";
  }
}

static std::string _DebugLinkSource(HgiShaderProgramHandle const &program)
{
  std::string result;
  result = TfStringPrintf("==== Source Program ID=%p\nBEGIN_DUMP\n", program.Get());

  for (HgiShaderFunctionHandle fn : program->GetShaderFunctions()) {
    HgiShaderFunctionDesc const &desc = fn->GetDescriptor();
    result.append(_GetShaderType(desc.shaderStage));
    result.append(desc.shaderCode);
  }

  result += "END DUMP\n";

  return result;
}

bool HdPhGLSLProgram::Link()
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  if (_programDesc.shaderFunctions.empty()) {
    TF_CODING_ERROR("At least one shader has to be compiled before linking.");
    return false;
  }

  Hgi *const hgi = _registry->GetHgi();

  // Create the shader program.
  if (_program) {
    hgi->DestroyShaderProgram(&_program);
  }
  _program = hgi->CreateShaderProgram(_programDesc);

  bool success = _program->IsValid();
  if (!success) {
    std::string const &logString = _program->GetCompileErrors();
    TF_WARN("Failed to link shader: %s", logString.c_str());

    if (TfDebug::IsEnabled(HDPH_DUMP_FAILING_SHADER_SOURCE)) {
      std::cout << _DebugLinkSource(_program) << std::flush;
    }
  }

  return success;
}

bool HdPhGLSLProgram::Validate() const
{
  if (!_program || !_program->IsValid()) {
    return false;
  }

  return true;
}

HdPhGLSLProgramSharedPtr HdPhGLSLProgram::GetComputeProgram(TfToken const &shaderToken,
                                                            HdPhResourceRegistry *resourceRegistry)
{
  return GetComputeProgram(HdPhPackageComputeShader(), shaderToken, resourceRegistry);
}

HdPhGLSLProgramSharedPtr HdPhGLSLProgram::GetComputeProgram(TfToken const &shaderFileName,
                                                            TfToken const &shaderToken,
                                                            HdPhResourceRegistry *resourceRegistry)
{
  // Find the program from registry
  HdInstance<HdPhGLSLProgramSharedPtr> programInstance = resourceRegistry->RegisterGLSLProgram(
      HdPhGLSLProgram::ComputeHash(shaderToken));

  if (programInstance.IsFirstInstance()) {
    // if not exists, create new one
    HdPhGLSLProgramSharedPtr newProgram(
        new HdPhGLSLProgram(HdTokens->computeShader, resourceRegistry));

    HioGlslfx glslfx(shaderFileName);
    std::string errorString;
    if (!glslfx.IsValid(&errorString)) {
      TF_CODING_ERROR("Failed to parse " + shaderFileName.GetString() + ": " + errorString);
      return HdPhGLSLProgramSharedPtr();
    }
    std::string version = "#version 430\n";
    if (!newProgram->CompileShader(HgiShaderStageCompute,
                                   version + glslfx.GetSource(shaderToken))) {
      TF_CODING_ERROR("Fail to compile " + shaderToken.GetString());
      return HdPhGLSLProgramSharedPtr();
    }
    if (!newProgram->Link()) {
      TF_CODING_ERROR("Fail to link " + shaderToken.GetString());
      return HdPhGLSLProgramSharedPtr();
    }
    programInstance.SetValue(newProgram);
  }
  return programInstance.GetValue();
}

WABI_NAMESPACE_END
