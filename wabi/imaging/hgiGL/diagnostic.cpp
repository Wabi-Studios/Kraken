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
// Diagnostic.cpp
//

#include "wabi/imaging/garch/glApi.h"

#include "wabi/imaging/hgiGL/diagnostic.h"

#include "wabi/base/tf/debug.h"
#include "wabi/base/tf/diagnostic.h"
#include "wabi/base/tf/envSetting.h"
#include "wabi/base/tf/registryManager.h"
#include "wabi/base/tf/stackTrace.h"
#include "wabi/base/tf/stringUtils.h"

WABI_NAMESPACE_BEGIN

TF_DEFINE_ENV_SETTING(HGIGL_DEBUG, 0, "Enable OpenGL debugging for HgiGL");

TF_DEBUG_CODES(HGIGL_DEBUG_ERROR_STACKTRACE);

TF_REGISTRY_FUNCTION(TfDebug)
{
  TF_DEBUG_ENVIRONMENT_SYMBOL(HGIGL_DEBUG_ERROR_STACKTRACE, "HgiGL dump stack trace on GL error");
}

bool HgiGLDebugEnabled()
{
#if defined(GL_KHR_debug)
  static bool _v = TfGetEnvSetting(HGIGL_DEBUG) == 1;
  return _v;
#else
  return false;
#endif
}

void HgiGLPostPendingGLErrors(std::string const &where)
{
  bool foundError = false;
  GLenum error;
  // Protect from doing infinite looping when glGetError
  // is called from an invalid context.
  int watchDogCount = 0;
  while ((watchDogCount++ < 256) && ((error = glGetError()) != GL_NO_ERROR))
  {
    foundError = true;
    const GLubyte *errorString = gluErrorString(error);

    std::ostringstream errorMessage;
    if (!errorString)
    {
      errorMessage << "GL error code: 0x" << std::hex << error << std::dec;
    } else
    {
      errorMessage << "GL error: " << errorString;
    }

    if (!where.empty())
    {
      errorMessage << ", reported from " << where;
    }

    TF_DEBUG(HGIGL_DEBUG_ERROR_STACKTRACE).Msg(errorMessage.str() + "\n");

    TF_RUNTIME_ERROR(errorMessage.str());
  }
  if (foundError)
  {
    TF_DEBUG(HGIGL_DEBUG_ERROR_STACKTRACE)
      .Msg(TfStringPrintf("==== GL Error Stack ====\n%s\n", TfGetStackTrace().c_str()));
  }
}

static void _HgiGLErrorCallback(int e, const char *m)
{
  std::string err = "GL Error code: " + std::to_string(e) + " Msg: " + m +
                    " Func: " + __ARCH_PRETTY_FUNCTION__;
  HgiGLPostPendingGLErrors(err);
}

static void _HgiGLGL4DbgCallback(GLenum src,
                                 GLenum type,
                                 GLuint id,
                                 GLenum severity,
                                 GLsizei length,
                                 const GLchar *msg,
                                 GLvoid *user)
{
  switch (id)
  {
    case 131218:
      return;  // 'Shader re-compiled due to GL state mismatch'
    case 131185:
      return;  // 'buffer map details'
    case 131076:
      return;  // 'vert attrb array small' (when no mesh loaded)
    default:
      break;
  }
  _HgiGLErrorCallback(id, msg);
}

void HgiGLSetupGL4Debug()
{
  if (!HgiGLDebugEnabled())
    return;

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC)_HgiGLGL4DbgCallback, 0);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

bool HgiGLMeetsMinimumRequirements()
{
  const char *glVersionStr = (const char *)glGetString(GL_VERSION);

  // GL hasn't been initialized
  if (glVersionStr == nullptr)
    return false;

  int glVersion = 0;

  const char *dot = strchr(glVersionStr, '.');
  if (TF_VERIFY((dot && dot != glVersionStr), "Can't parse GL_VERSION %s", glVersionStr))
  {
    // GL_VERSION = "4.5.0 <vendor> <version>"
    //              "4.1 <vendor-os-ver> <version>"
    //              "4.1 <vendor-os-ver>"
    int major = std::max(0, std::min(9, *(dot - 1) - '0'));
    int minor = std::max(0, std::min(9, *(dot + 1) - '0'));
    glVersion = major * 100 + minor * 10;
  }

  return (glVersion >= 450);
}

void HgiGLObjectLabel(const uint32_t identifier, const uint32_t name, const std::string &label)
{
  GLint maxLength;
  glGetIntegerv(GL_MAX_LABEL_LENGTH, &maxLength);

  glObjectLabel(identifier,
                name,
                std::min(label.size(),
                         size_t(maxLength - 1)),  // Account for 0-terminator.
                label.c_str());
}

WABI_NAMESPACE_END
