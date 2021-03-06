//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
// info.cpp
//

#include "wabi/imaging/garch/glApi.h"

#include "wabi/imaging/glf/info.h"
#include "wabi/imaging/glf/glContext.h"

#include "wabi/base/tf/stringUtils.h"

#include <set>
#include <string>
#include <vector>

WABI_NAMESPACE_BEGIN


using std::set;
using std::string;
using std::vector;

static set<string> Glf_BuildAvailableExtensions()
{
  GlfSharedGLContextScopeHolder sharedContextScopeHolder;

  set<string> availableExtensions;

  // Get the available extensions from OpenGL if we haven't yet.
  if (const char *extensions = (const char *)glGetString(GL_EXTENSIONS)) {
    const vector<string> extensionsList = TfStringTokenize(extensions);
    for (std::string const &extension : extensionsList) {
      availableExtensions.insert(extension);
    }
  }
  return availableExtensions;
}

bool GlfHasExtensions(string const &queryExtensions)
{
  static set<string> availableExtensions = Glf_BuildAvailableExtensions();

  // Tokenize the queried extensions.
  const vector<string> extensionsList = TfStringTokenize(queryExtensions);

  // Return false if any queried extension is not available.
  for (std::string const &extension : extensionsList) {
    if (!availableExtensions.count(extension)) {
      return false;
    }
  }

  // All queried extensions were found.
  return true;
}


WABI_NAMESPACE_END
