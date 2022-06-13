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
#include "wabi/usd/usdPhysics/revoluteJoint.h"
#include "wabi/usd/usd/schemaRegistry.h"
#include "wabi/usd/usd/typed.h"

#include "wabi/usd/sdf/types.h"
#include "wabi/usd/sdf/assetPath.h"

WABI_NAMESPACE_BEGIN

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<UsdPhysicsRevoluteJoint, TfType::Bases<UsdPhysicsJoint>>();

  // Register the usd prim typename as an alias under UsdSchemaBase. This
  // enables one to call
  // TfType::Find<UsdSchemaBase>().FindDerivedByName("PhysicsRevoluteJoint")
  // to find TfType<UsdPhysicsRevoluteJoint>, which is how IsA queries are
  // answered.
  TfType::AddAlias<UsdSchemaBase, UsdPhysicsRevoluteJoint>("PhysicsRevoluteJoint");
}

/* virtual */
UsdPhysicsRevoluteJoint::~UsdPhysicsRevoluteJoint() {}

/* static */
UsdPhysicsRevoluteJoint UsdPhysicsRevoluteJoint::Get(const UsdStagePtr &stage, const SdfPath &path)
{
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdPhysicsRevoluteJoint();
  }
  return UsdPhysicsRevoluteJoint(stage->GetPrimAtPath(path));
}

/* static */
UsdPhysicsRevoluteJoint UsdPhysicsRevoluteJoint::Define(const UsdStagePtr &stage,
                                                        const SdfPath &path)
{
  static TfToken usdPrimTypeName("PhysicsRevoluteJoint");
  if (!stage) {
    TF_CODING_ERROR("Invalid stage");
    return UsdPhysicsRevoluteJoint();
  }
  return UsdPhysicsRevoluteJoint(stage->DefinePrim(path, usdPrimTypeName));
}

/* virtual */
UsdSchemaKind UsdPhysicsRevoluteJoint::GetSchemaKind() const
{
  return UsdPhysicsRevoluteJoint::schemaKind;
}

/* static */
const TfType &UsdPhysicsRevoluteJoint::GetStaticTfType()
{
  static TfType tfType = TfType::Find<UsdPhysicsRevoluteJoint>();
  return tfType;
}

/* static */
bool UsdPhysicsRevoluteJoint::IsTypedSchema()
{
  static bool isTyped = GetStaticTfType().IsA<UsdTyped>();
  return isTyped;
}

/* virtual */
const TfType &UsdPhysicsRevoluteJoint::GetTfType() const
{
  return GetStaticTfType();
}

UsdAttribute UsdPhysicsRevoluteJoint::GetAxisAttr() const
{
  return GetPrim().GetAttribute(UsdPhysicsTokens->physicsAxis);
}

UsdAttribute UsdPhysicsRevoluteJoint::CreateAxisAttr(VtValue const &defaultValue,
                                                     bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdPhysicsTokens->physicsAxis,
                                    SdfValueTypeNames->Token,
                                    /* custom = */ false,
                                    SdfVariabilityUniform,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdPhysicsRevoluteJoint::GetLowerLimitAttr() const
{
  return GetPrim().GetAttribute(UsdPhysicsTokens->physicsLowerLimit);
}

UsdAttribute UsdPhysicsRevoluteJoint::CreateLowerLimitAttr(VtValue const &defaultValue,
                                                           bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdPhysicsTokens->physicsLowerLimit,
                                    SdfValueTypeNames->Float,
                                    /* custom = */ false,
                                    SdfVariabilityVarying,
                                    defaultValue,
                                    writeSparsely);
}

UsdAttribute UsdPhysicsRevoluteJoint::GetUpperLimitAttr() const
{
  return GetPrim().GetAttribute(UsdPhysicsTokens->physicsUpperLimit);
}

UsdAttribute UsdPhysicsRevoluteJoint::CreateUpperLimitAttr(VtValue const &defaultValue,
                                                           bool writeSparsely) const
{
  return UsdSchemaBase::_CreateAttr(UsdPhysicsTokens->physicsUpperLimit,
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
const TfTokenVector &UsdPhysicsRevoluteJoint::GetSchemaAttributeNames(bool includeInherited)
{
  static TfTokenVector localNames = {
    UsdPhysicsTokens->physicsAxis,
    UsdPhysicsTokens->physicsLowerLimit,
    UsdPhysicsTokens->physicsUpperLimit,
  };
  static TfTokenVector allNames = _ConcatenateAttributeNames(
    UsdPhysicsJoint::GetSchemaAttributeNames(true),
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
