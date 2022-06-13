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
#include "wabi/usd/usdPhysics/scene.h"
#include "wabi/usd/usd/schemaRegistry.h"
#include "wabi/usd/usd/typed.h"

#include "wabi/usd/sdf/types.h"
#include "wabi/usd/sdf/assetPath.h"

WABI_NAMESPACE_BEGIN

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<UsdPhysicsScene, TfType::Bases<UsdTyped>>();

  // Register the usd prim typename as an alias under UsdSchemaBase. This
  // enables one to call
  // TfType::Find<UsdSchemaBase>().FindDerivedByName("PhysicsScene")
  // to find TfType<UsdPhysicsScene>, which is how IsA queries are
  // answered.
  TfType::AddAlias<UsdSchemaBase, UsdPhysicsScene>("PhysicsScene");
}

/* virtual */
UsdPhysicsScene::~UsdPhysicsScene() {}

/* static */
UsdPhysicsScene UsdPhysicsScene::Get(const UsdStagePtr &stage, const SdfPath &path)
{
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdPhysicsScene();
  }
  return UsdPhysicsScene(stage->GetPrimAtPath(path));
}

/* static */
UsdPhysicsScene UsdPhysicsScene::Define(const UsdStagePtr &stage, const SdfPath &path)
{
  static TfToken usdPrimTypeName("PhysicsScene");
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdPhysicsScene();
  }
  return UsdPhysicsScene(stage->DefinePrim(path, usdPrimTypeName));
}

/* virtual */
UsdSchemaKind UsdPhysicsScene::GetSchemaKind() const
{
  return UsdPhysicsScene::schemaKind;
}

/* static */
const TfType &UsdPhysicsScene::GetStaticTfType()
{
  static TfType tfType = TfType::Find<UsdPhysicsScene>();
  return tfType;
}

/* static */
bool UsdPhysicsScene::IsTypedSchema()
{
  static bool isTyped = GetStaticTfType().IsA<UsdTyped>();
  return isTyped;
}

/* virtual */
const TfType &UsdPhysicsScene::GetTfType() const
{
  return GetStaticTfType();
}

UsdAttribute UsdPhysicsScene::GetGravityDirectionAttr() const
{
  return GetPrim().GetAttribute(UsdPhysicsTokens->physicsGravityDirection);
}

UsdAttribute UsdPhysicsScene::CreateGravityDirectionAttr(VtValue const &defaultValue,
                                                         bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdPhysicsTokens->physicsGravityDirection,
                                    SdfValueTypeNames->Vector3f,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdPhysicsScene::GetGravityMagnitudeAttr() const
{
  return GetPrim().GetAttribute(UsdPhysicsTokens->physicsGravityMagnitude);
}

UsdAttribute UsdPhysicsScene::CreateGravityMagnitudeAttr(VtValue const &defaultValue,
                                                         bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdPhysicsTokens->physicsGravityMagnitude,
                                    SdfValueTypeNames->Float,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
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
const TfTokenVector &UsdPhysicsScene::GetSchemaAttributeNames(bool includeInherited)
{
  static TfTokenVector localNames = {
    UsdPhysicsTokens->physicsGravityDirection,
    UsdPhysicsTokens->physicsGravityMagnitude,
  };
  static TfTokenVector allNames = _ConcatenateAttributeNames(
    UsdTyped::GetSchemaAttributeNames(true),
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
