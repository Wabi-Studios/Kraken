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

#include "debugCodes.h"

#include "wabi/base/tf/debug.h"
#include "wabi/base/tf/registryManager.h"

WABI_NAMESPACE_BEGIN

TF_REGISTRY_FUNCTION(TfDebug)
{
  TF_DEBUG_ENVIRONMENT_SYMBOL(HD_RPR_DEBUG_CONTEXT_CREATION, "hdRpr context creation");
  TF_DEBUG_ENVIRONMENT_SYMBOL(HD_RPR_DEBUG_CORE_UNSUPPORTED_ERROR,
                              "hdRpr signal about unsupported errors");
}

WABI_NAMESPACE_END
