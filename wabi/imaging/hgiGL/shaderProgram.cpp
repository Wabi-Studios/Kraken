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
#include "wabi/imaging/garch/glApi.h"

#include "wabi/imaging/hgiGL/diagnostic.h"
#include "wabi/imaging/hgiGL/shaderFunction.h"
#include "wabi/imaging/hgiGL/shaderProgram.h"

#include "wabi/base/tf/diagnostic.h"

WABI_NAMESPACE_BEGIN

HgiGLShaderProgram::HgiGLShaderProgram(HgiShaderProgramDesc const &desc)
  : HgiShaderProgram(desc),
    _programId(0),
    _programByteSize(0),
    _uniformBuffer(0),
    _uboByteSize(0)
{
  _programId = glCreateProgram();

  if (!_descriptor.debugName.empty())
  {
    HgiGLObjectLabel(GL_PROGRAM, _programId, _descriptor.debugName);
  }

  for (HgiShaderFunctionHandle const &shd : desc.shaderFunctions)
  {
    HgiGLShaderFunction *glShader = static_cast<HgiGLShaderFunction *>(shd.Get());
    uint32_t id = glShader->GetShaderId();
    TF_VERIFY(id > 0, "Invalid shader provided to program");
    glAttachShader(_programId, id);
  }
  glLinkProgram(_programId);

  // Grab compile errors
  GLint status;
  glGetProgramiv(_programId, GL_LINK_STATUS, &status);
  if (status != GL_TRUE)
  {
    int logSize = 0;
    glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &logSize);
    _errors.resize(logSize + 1);
    glGetProgramInfoLog(_programId, logSize, nullptr, &_errors[0]);
    glDeleteProgram(_programId);
    _programId = 0;
  } else
  {
    GLint size;
    glGetProgramiv(_programId, GL_PROGRAM_BINARY_LENGTH, &size);
    _programByteSize = (size_t)size;
  }

  glCreateBuffers(1, &_uniformBuffer);

  HGIGL_POST_PENDING_GL_ERRORS();
}

HgiGLShaderProgram::~HgiGLShaderProgram()
{
  glDeleteProgram(_programId);
  _programId = 0;
  glDeleteBuffers(1, &_uniformBuffer);
  _uniformBuffer = 0;
  HGIGL_POST_PENDING_GL_ERRORS();
}

HgiShaderFunctionHandleVector const &HgiGLShaderProgram::GetShaderFunctions() const
{
  return _descriptor.shaderFunctions;
}

bool HgiGLShaderProgram::IsValid() const
{
  return _programId > 0 && _errors.empty();
}

std::string const &HgiGLShaderProgram::GetCompileErrors()
{
  return _errors;
}

size_t HgiGLShaderProgram::GetByteSizeOfResource() const
{
  return _programByteSize + _uboByteSize;
}

uint64_t HgiGLShaderProgram::GetRawResource() const
{
  return (uint64_t)_programId;
}

uint32_t HgiGLShaderProgram::GetProgramId() const
{
  return _programId;
}

uint32_t HgiGLShaderProgram::GetUniformBuffer(size_t sizeHint)
{
  _uboByteSize = sizeHint;
  return _uniformBuffer;
}

WABI_NAMESPACE_END
