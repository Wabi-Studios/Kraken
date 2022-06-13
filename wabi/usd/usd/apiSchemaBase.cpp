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

#include "wabi/usd/usd/schemaRegistry.h"
#include "wabi/usd/usd/typed.h"

#include "wabi/usd/sdf/assetPath.h"
#include "wabi/usd/sdf/types.h"
#include "wabi/usd/usd/apiSchemaBase.h"

WABI_NAMESPACE_BEGIN

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<UsdAPISchemaBase,
        TfType::Bases< UsdSchemaBase > >();
    
}

/* virtual */
UsdAPISchemaBase::~UsdAPISchemaBase()
{
}


/* virtual */
UsdSchemaKind UsdAPISchemaBase::_GetSchemaKind() const
{
    return UsdAPISchemaBase::schemaKind;
}

/* static */
const TfType &
UsdAPISchemaBase::_GetStaticTfType()
{
    static TfType tfType = TfType::Find<UsdAPISchemaBase>();
    return tfType;
}

/* static */
bool 
UsdAPISchemaBase::_IsTypedSchema()
{
    static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
    return isTyped;
}

/* virtual */
const TfType &
UsdAPISchemaBase::_GetTfType() const
{
    return _GetStaticTfType();
}

/*static*/
const TfTokenVector&
UsdAPISchemaBase::GetSchemaAttributeNames(bool includeInherited)
{
    static TfTokenVector localNames;
    static TfTokenVector allNames =
        UsdSchemaBase::GetSchemaAttributeNames(true);

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

#include "wabi/usd/usd/tokens.h"

WABI_NAMESPACE_BEGIN


/* virtual */
bool 
UsdAPISchemaBase::_IsCompatible() const
{
    if (!UsdSchemaBase::_IsCompatible())
        return false;

    // This virtual function call tells us whether we're an applied 
    // API schema. For applied API schemas, we'd like to check whether 
    // the API schema has been applied properly on the prim.
    if (IsAppliedAPISchema()) {
        if (IsMultipleApplyAPISchema()) {
            if (_instanceName.IsEmpty() ||
                !GetPrim()._HasMultiApplyAPI(_GetTfType(), _instanceName)) {
                return false;
            }
        } else {
            if (!GetPrim()._HasSingleApplyAPI(_GetTfType())) {
                return false;
            }
        }
    }

    return true;
}

WABI_NAMESPACE_END
