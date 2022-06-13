/************************************************************************
Copyright 2020 Advanced Micro Devices, Inc
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
************************************************************************/

#include "wabi/base/tf/registryManager.h"
#include "wabi/base/tf/scriptModuleLoader.h"
#include "wabi/base/tf/token.h"
#include "wabi/wabi.h"

#include <vector>

WABI_NAMESPACE_BEGIN

TF_REGISTRY_FUNCTION(TfScriptModuleLoader)
{
  // List of direct dependencies for this library.
  const std::vector<TfToken> reqs = {
    TfToken("tf"),
  };
  TfScriptModuleLoader::GetInstance().RegisterLibrary(TfToken("rprUsd"),
                                                      TfToken("rpr.RprUsd"),
                                                      reqs);
}

WABI_NAMESPACE_END
