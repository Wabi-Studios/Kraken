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
#include "wabi/usd/usdLux/listAPI.h"
#include "wabi/usd/usd/schemaRegistry.h"
#include "wabi/usd/usd/tokens.h"
#include "wabi/usd/usd/typed.h"

#include "wabi/usd/sdf/assetPath.h"
#include "wabi/usd/sdf/types.h"

WABI_NAMESPACE_BEGIN

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<UsdLuxListAPI, TfType::Bases<UsdAPISchemaBase>>();
}

TF_DEFINE_PRIVATE_TOKENS(_schemaTokens, (ListAPI));

/* virtual */
UsdLuxListAPI::~UsdLuxListAPI()
{}

/* static */
UsdLuxListAPI UsdLuxListAPI::Get(const UsdStagePtr &stage, const SdfPath &path)
{
  if (!stage)
  {
    TF_CODING_ERROR("Invalid stage");
    return UsdLuxListAPI();
  }
  return UsdLuxListAPI(stage->GetPrimAtPath(path));
}

/* virtual */
UsdSchemaKind UsdLuxListAPI::GetSchemaKind() const
{
  return UsdLuxListAPI::schemaKind;
}

/* static */
UsdLuxListAPI UsdLuxListAPI::Apply(const UsdPrim &prim)
{
  if (prim.ApplyAPI<UsdLuxListAPI>())
  {
    return UsdLuxListAPI(prim);
  }
  return UsdLuxListAPI();
}

/* static */
const TfType &UsdLuxListAPI::GetStaticTfType()
{
  static TfType tfType = TfType::Find<UsdLuxListAPI>();
  return tfType;
}

/* static */
bool UsdLuxListAPI::IsTypedSchema()
{
  static bool isTyped = GetStaticTfType().IsA<UsdTyped>();
  return isTyped;
}

/* virtual */
const TfType &UsdLuxListAPI::GetTfType() const
{
  return GetStaticTfType();
}

UsdAttribute UsdLuxListAPI::GetLightListCacheBehaviorAttr() const
{
  return GetPrim().GetAttribute(UsdLuxTokens->lightListCacheBehavior);
}

UsdAttribute UsdLuxListAPI::CreateLightListCacheBehaviorAttr(VtValue const &defaultValue,
                                                             bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdLuxTokens->lightListCacheBehavior,
                                    SdfValueTypeNames->Token,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdRelationship UsdLuxListAPI::GetLightListRel() const
{
  return GetPrim().GetRelationship(UsdLuxTokens->lightList);
}

UsdRelationship UsdLuxListAPI::CreateLightListRel() const
{
  return GetPrim().CreateRelationship(UsdLuxTokens->lightList,
                                      /* custom = */ false);
}

namespace
{
  static inline TfTokenVector _ConcatenateAttributeNames(const TfTokenVector &left,
                                                         const TfTokenVector &right)
  {
    TfTokenVector result;
    result.reserve(left.size() + right.size());
    result.insert(result.end(), left.begin(), left.end());
    result.insert(result.end(), right.begin(), right.end());
    return result;
  }
}  // namespace

/*static*/
const TfTokenVector &UsdLuxListAPI::GetSchemaAttributeNames(bool includeInherited)
{
  static TfTokenVector localNames = {
    UsdLuxTokens->lightListCacheBehavior,
  };
  static TfTokenVector allNames = _ConcatenateAttributeNames(UsdAPISchemaBase::GetSchemaAttributeNames(true),
                                                             localNames);

  if (includeInherited)
    return allNames;
  else
    return localNames;
}

WABI_NAMESPACE_END

// ===================================================================== //
// Feel free to add custom code below this line. It will be preserved by
// the code generator.
//
// Just remember to wrap code in the appropriate delimiters:
// 'WABI_NAMESPACE_BEGIN', 'WABI_NAMESPACE_END'.
// ===================================================================== //
// --(BEGIN CUSTOM CODE)--

#include "wabi/usd/usd/primRange.h"
#include "wabi/usd/usdLux/light.h"
#include "wabi/usd/usdLux/lightFilter.h"

WABI_NAMESPACE_BEGIN

TF_REGISTRY_FUNCTION(TfEnum)
{
  TF_ADD_ENUM_NAME(UsdLuxListAPI::ComputeModeConsultModelHierarchyCache, "Consult lightList cache");
  TF_ADD_ENUM_NAME(UsdLuxListAPI::ComputeModeIgnoreCache, "Ignore lightList cache");
}

static void _Traverse(const UsdPrim &prim, UsdLuxListAPI::ComputeMode mode, SdfPathSet *lights)
{
  // If requested, check lightList cache.
  if (mode == UsdLuxListAPI::ComputeModeConsultModelHierarchyCache &&
      prim.GetPath().IsPrimPath() /* no cache on pseudoRoot */)
  {
    UsdLuxListAPI listAPI(prim);
    TfToken cacheBehavior;
    if (listAPI.GetLightListCacheBehaviorAttr().Get(&cacheBehavior))
    {
      if (cacheBehavior == UsdLuxTokens->consumeAndContinue || cacheBehavior == UsdLuxTokens->consumeAndHalt)
      {
        // Check stored lightList.
        UsdRelationship rel = listAPI.GetLightListRel();
        SdfPathVector targets;
        rel.GetForwardedTargets(&targets);
        lights->insert(targets.begin(), targets.end());
        if (cacheBehavior == UsdLuxTokens->consumeAndHalt)
        {
          return;
        }
      }
    }
  }
  // Accumulate discovered prims.
  if (prim.IsA<UsdLuxLight>() || prim.IsA<UsdLuxLightFilter>())
  {
    lights->insert(prim.GetPath());
  }
  // Traverse descendants.
  auto flags = UsdPrimIsActive && !UsdPrimIsAbstract && UsdPrimIsDefined;
  if (mode == UsdLuxListAPI::ComputeModeConsultModelHierarchyCache)
  {
    // When consulting the cache we only traverse model hierarchy.
    flags = flags && UsdPrimIsModel;
  }
  for (const UsdPrim &child : prim.GetFilteredChildren(UsdTraverseInstanceProxies(flags)))
  {
    _Traverse(child, mode, lights);
  }
}

SdfPathSet UsdLuxListAPI::ComputeLightList(UsdLuxListAPI::ComputeMode mode) const
{
  SdfPathSet result;
  _Traverse(GetPrim(), mode, &result);
  return result;
}

void UsdLuxListAPI::StoreLightList(const SdfPathSet &lights) const
{
  SdfPathVector targets;
  for (const SdfPath &p : lights)
  {
    if (p.IsAbsolutePath() && !p.HasPrefix(GetPath()))
    {
      // Light path does not have this prim as a prefix; ignore.
      continue;
    }
    targets.push_back(p);
  }
  CreateLightListRel().SetTargets(targets);
  CreateLightListCacheBehaviorAttr().Set(UsdLuxTokens->consumeAndContinue);
}

void UsdLuxListAPI::InvalidateLightList() const
{
  CreateLightListCacheBehaviorAttr().Set(UsdLuxTokens->ignore);
}

WABI_NAMESPACE_END
