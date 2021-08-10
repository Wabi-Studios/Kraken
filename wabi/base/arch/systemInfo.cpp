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

#include "wabi/wabi.h"

#include "wabi/base/arch/error.h"
#include "wabi/base/arch/fileSystem.h"
#include "wabi/base/arch/systemInfo.h"

#include <cstdlib>
#include <functional>
#include <limits>
#include <filesystem>

#if defined(ARCH_OS_LINUX)

#  include <sys/stat.h>
#  include <sys/types.h>
#  include <unistd.h>

#elif defined(ARCH_OS_DARWIN)

#  include <mach-o/dyld.h>
#  include <unistd.h>

#elif defined(ARCH_OS_WINDOWS)

#  include <Windows.h>
#  include <direct.h>
#  include <winrt/base.h>
#  include <winrt/Windows.ApplicationModel.h>
#  include <winrt/Windows.Foundation.h>
#  include <winrt/Windows.Storage.h>
#  define getcwd(buffer_, size_) _getcwd(buffer_, size_)

namespace MICROSOFT = winrt;

using namespace MICROSOFT;
using namespace MICROSOFT::Windows;
using namespace MICROSOFT::Windows::Foundation;
using namespace MICROSOFT::Windows::ApplicationModel;
using namespace MICROSOFT::Windows::Storage;

namespace fs = std::filesystem;

#else

#  error Unknown architecture.

#endif

using std::string;

WABI_NAMESPACE_BEGIN

std::string ArchGetCwd()
{
  // Try a fixed size buffer.
  char buffer[ARCH_PATH_MAX];
  if (getcwd(buffer, ARCH_PATH_MAX))
  {
    return std::string(buffer);
  }

  // Let the system allocate the buffer.
  if (char *buf = getcwd(NULL, 0))
  {
    std::string result(buf);
    free(buf);
    return result;
  }

  ARCH_WARNING("can't determine working directory");
  return ".";
}

namespace
{

  // Getting the executable path requires a dynamically allocated buffer
  // on all platforms.  This helper function handles the allocation.
  static std::string _DynamicSizedRead(size_t initialSize,
                                       const std::function<bool(char *, size_t *)> &callback)
  {
    // Make a buffer for the data.
    std::unique_ptr<char[]> buffer;
    buffer.reset(new char[initialSize]);

    // Repeatedly invoke the callback with our buffer until it's big enough.
    size_t size = initialSize;
    while (!callback(buffer.get(), &size))
    {
      if (size == std::numeric_limits<size_t>::max())
      {
        // callback is never going to succeed.
        return std::string();
      }
      buffer.reset(new char[size]);
    }

    // Make a string.
    return std::string(buffer.get());
  }

}  // namespace

std::string ArchGetExecutablePath()
{
#if defined(ARCH_OS_LINUX)

  // On Linux the executable path is retrieved from the /proc/self/exe
  // symlink.
  return _DynamicSizedRead(ARCH_PATH_MAX, [](char *buffer, size_t *size) {
    const ssize_t n = readlink("/proc/self/exe", buffer, *size);
    if (n == -1)
    {
      ARCH_WARNING(
        "Unable to read /proc/self/exe to obtain "
        "executable path");
      *size = std::numeric_limits<size_t>::max();
      return false;
    } else if (static_cast<size_t>(n) >= *size)
    {
      // Find out how much space we need.
      struct stat sb;
      if (lstat("/proc/self/exe", &sb) == 0)
      {
        *size = sb.st_size + 1;
      } else
      {
        // Try iterating on the size.
        *size *= 2;
      }
      return false;
    } else
    {
      buffer[n] = '\0';
      return true;
    }
  });

#elif defined(ARCH_OS_DARWIN)

  // On Darwin _NSGetExecutablePath() returns the executable path.
  return _DynamicSizedRead(ARCH_PATH_MAX, [](char *buffer, size_t *size) {
    uint32_t bufsize = *size;
    if (_NSGetExecutablePath(buffer, &bufsize) == -1)
    {
      // We're told the correct size.
      *size = bufsize;
      return false;
    } else
    {
      return true;
    }
  });

#elif defined(ARCH_OS_WINDOWS)

  return MICROSOFT::to_string(Package::Current().InstalledLocation().Path());

#if 0
  /**
   * On Windows GetModuleFileName()
   * Will no longer work on UWP.
   * 
   * Leaving this here for posterity,
   * as well as historical reasons. */
  return _DynamicSizedRead(ARCH_PATH_MAX, [](char *buffer, size_t *size) {
    DWORD nSize = *size;
    const DWORD n = GetModuleFileName(NULL, (LPWSTR)buffer, nSize);
    if (n == 0)
    {
      ARCH_WARNING(
        "Unable to read GetModuleFileName() to obtain "
        "executable path");
      *size = std::numeric_limits<size_t>::max();
      return false;
    } else if (n >= nSize)
    {
      // We have to iterate to find a suitable size.
      *size *= 2;
      return false;
    } else
    {
      return true;
    }
  });
#endif

#endif
}

int ArchGetPageSize()
{
#if defined(ARCH_OS_LINUX) || defined(ARCH_OS_DARWIN)
  return sysconf(_SC_PAGE_SIZE);
#elif defined(ARCH_OS_WINDOWS)
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return info.dwPageSize;
#else
#  error Unknown architecture.
#endif
}

WABI_NAMESPACE_END
