//
// Copyright 2020 Pixar
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
// Diagnostic.cpp
//


#include <Metal/Metal.h>

#include "wabi/imaging/hgiMetal/diagnostic.h"

#include "wabi/base/tf/debug.h"
#include "wabi/base/tf/diagnostic.h"
#include "wabi/base/tf/envSetting.h"
#include "wabi/base/tf/registryManager.h"
#include "wabi/base/tf/stackTrace.h"
#include "wabi/base/tf/stringUtils.h"

WABI_NAMESPACE_BEGIN

TF_DEFINE_ENV_SETTING(HGIMETAL_DEBUG, 0,
                      "Enable Metal debugging for HgiMetal");

TF_DEBUG_CODES(
    HGIMETAL_DEBUG_ERROR_STACKTRACE
);

TF_REGISTRY_FUNCTION(TfDebug)
{
    TF_DEBUG_ENVIRONMENT_SYMBOL(HGIMETAL_DEBUG_ERROR_STACKTRACE,
        "HgiMetal dump stack trace on Metal error");
}

bool
HgiMetalDebugEnabled()
{
    static bool _v = TfGetEnvSetting(HGIMETAL_DEBUG) == 1;
    return _v;
}

void
HgiMetalPostPendingMetalErrors(std::string const & where)
{
}

void
HgiMetalSetupMetalDebug()
{
    if (!HgiMetalDebugEnabled()) return;
}

WABI_NAMESPACE_END

