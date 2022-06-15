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
#include "wabi/usd/usd/tokens.h"

WABI_NAMESPACE_BEGIN

UsdTokensType::UsdTokensType()
  : apiSchemas("apiSchemas", TfToken::Immortal),
    clips("clips", TfToken::Immortal),
    clipSets("clipSets", TfToken::Immortal),
    collection("collection", TfToken::Immortal),
    collection_MultipleApplyTemplate_Excludes("collection:__INSTANCE_NAME__:excludes",
                                              TfToken::Immortal),
    collection_MultipleApplyTemplate_ExpansionRule("collection:__INSTANCE_NAME__:expansionRule",
                                                   TfToken::Immortal),
    collection_MultipleApplyTemplate_IncludeRoot("collection:__INSTANCE_NAME__:includeRoot",
                                                 TfToken::Immortal),
    collection_MultipleApplyTemplate_Includes("collection:__INSTANCE_NAME__:includes",
                                              TfToken::Immortal),
    exclude("exclude", TfToken::Immortal),
    expandPrims("expandPrims", TfToken::Immortal),
    expandPrimsAndProperties("expandPrimsAndProperties", TfToken::Immortal),
    explicitOnly("explicitOnly", TfToken::Immortal),
    fallbackPrimTypes("fallbackPrimTypes", TfToken::Immortal),
    allTokens({apiSchemas,
               clips,
               clipSets,
               collection,
               collection_MultipleApplyTemplate_Excludes,
               collection_MultipleApplyTemplate_ExpansionRule,
               collection_MultipleApplyTemplate_IncludeRoot,
               collection_MultipleApplyTemplate_Includes,
               exclude,
               expandPrims,
               expandPrimsAndProperties,
               explicitOnly,
               fallbackPrimTypes})
{}

TfStaticData<UsdTokensType> UsdTokens;

WABI_NAMESPACE_END
