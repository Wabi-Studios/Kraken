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

#include "wabi/usd/usd/tokens.h"

WABI_NAMESPACE_BEGIN

UsdTokensType::UsdTokensType()
  : apiSchemas("apiSchemas", TfToken::Immortal),
    clips("clips", TfToken::Immortal),
    clipSets("clipSets", TfToken::Immortal),
    collection("collection", TfToken::Immortal),
    exclude("exclude", TfToken::Immortal),
    excludes("excludes", TfToken::Immortal),
    expandPrims("expandPrims", TfToken::Immortal),
    expandPrimsAndProperties("expandPrimsAndProperties", TfToken::Immortal),
    expansionRule("expansionRule", TfToken::Immortal),
    explicitOnly("explicitOnly", TfToken::Immortal),
    fallbackPrimTypes("fallbackPrimTypes", TfToken::Immortal),
    includeRoot("includeRoot", TfToken::Immortal),
    includes("includes", TfToken::Immortal),
    allTokens({apiSchemas,
               clips,
               clipSets,
               collection,
               exclude,
               excludes,
               expandPrims,
               expandPrimsAndProperties,
               expansionRule,
               explicitOnly,
               fallbackPrimTypes,
               includeRoot,
               includes})
{}

TfStaticData<UsdTokensType> UsdTokens;

WABI_NAMESPACE_END
