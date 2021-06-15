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
#include "wabi/base/arch/env.h"
#include "wabi/base/arch/error.h"
#include "wabi/wabi.h"

#include <memory>
#include <regex>

#if defined(ARCH_OS_WINDOWS)
#  include <Windows.h>
#else
#  include <stdlib.h>
#endif

#if defined(ARCH_OS_DARWIN)
#  include <crt_externs.h>
#else
extern "C" char **environ;
#endif

WABI_NAMESPACE_BEGIN

bool ArchHasEnv(const std::string &name)
{
#if defined(ARCH_OS_WINDOWS)
  const DWORD size = GetEnvironmentVariable(name.c_str(), nullptr, 0);
  return size != 0 && size != ERROR_ENVVAR_NOT_FOUND;
#else
  return static_cast<bool>(getenv(name.c_str()));
#endif
}

std::string ArchGetEnv(const std::string &name)
{
#if defined(ARCH_OS_WINDOWS)
  const DWORD size = GetEnvironmentVariable(name.c_str(), nullptr, 0);
  if (size != 0) {
    std::unique_ptr<char[]> buffer(new char[size]);
    GetEnvironmentVariable(name.c_str(), buffer.get(), size);
    return std::string(buffer.get());
  }

#else
  if (const char *const result = getenv(name.c_str())) {
    return std::string(result);
  }
#endif

  return std::string();
}

bool ArchSetEnv(const std::string &name, const std::string &value, bool overwrite)
{
  // NOTE: Setting environment variables must be externally synchronized
  //       with other sets and gets to avoid race conditions.

#if defined(ARCH_OS_WINDOWS)
  if (!overwrite) {
    const DWORD size = GetEnvironmentVariable(name.c_str(), nullptr, 0);
    if (size == 0 || size != ERROR_ENVVAR_NOT_FOUND) {
      // Already exists or error.
      return true;
    }
  }
  return SetEnvironmentVariable(name.c_str(), value.c_str()) != 0;
#else
  return setenv(name.c_str(), value.c_str(), overwrite ? 1 : 0) == 0;
#endif
}

bool ArchRemoveEnv(const std::string &name)
{
#if defined(ARCH_OS_WINDOWS)
  return SetEnvironmentVariable(name.c_str(), nullptr) != 0;
#else
  return unsetenv(name.c_str()) == 0;
#endif
}

std::string ArchExpandEnvironmentVariables(const std::string &value)
{
#if defined(ARCH_OS_WINDOWS)
  static std::regex regex("\\%([^\\%]+)\\%");
#else
  static std::regex regex("\\$\\{([^}]+)\\}");
#endif

  std::string result = value;

  std::smatch match;
  while (std::regex_search(result, match, regex)) {
    // NOTE: g++'s standard library's replace() wants non-const iterators
    //       in violation of the standard.  We work around this by using
    //       indexes.
    const std::string::size_type pos = match[0].first - result.begin();
    const std::string::size_type count = match[0].second - match[0].first;
    result.replace(pos, count, ArchGetEnv(match[1].str()));
  }

  return result;
}

char **ArchEnviron()
{
#if defined(ARCH_OS_DARWIN)
  return *_NSGetEnviron();
#else
  return environ;
#endif
}

WABI_NAMESPACE_END
