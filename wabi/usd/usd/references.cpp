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
#include "wabi/usd/usd/references.h"
#include "wabi/usd/usd/common.h"
#include "wabi/usd/usd/listEditImpl.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

// ------------------------------------------------------------------------- //
// UsdReferences
// ------------------------------------------------------------------------- //

using _ListEditImpl = Usd_ListEditImpl<UsdReferences, SdfReferencesProxy>;

// The implementation doesn't define this function as it needs to be specialized
// so we implement it here.
template<> SdfReferencesProxy _ListEditImpl::_GetListEditorForSpec(const SdfPrimSpecHandle &spec)
{
  return spec->GetReferenceList();
}

bool UsdReferences::AddReference(const SdfReference &refIn, UsdListPosition position)
{
  return _ListEditImpl::Add(*this, refIn, position);
}

bool UsdReferences::AddReference(const std::string &assetPath,
                                 const SdfPath &primPath,
                                 const SdfLayerOffset &layerOffset,
                                 UsdListPosition position)
{
  SdfReference reference(assetPath, primPath, layerOffset);
  return AddReference(reference, position);
}

bool UsdReferences::AddReference(const std::string &assetPath,
                                 const SdfLayerOffset &layerOffset,
                                 UsdListPosition position)
{
  SdfReference reference(assetPath, SdfPath(), layerOffset);
  return AddReference(reference, position);
}

bool UsdReferences::AddInternalReference(const SdfPath &primPath,
                                         const SdfLayerOffset &layerOffset,
                                         UsdListPosition position)
{
  SdfReference reference(std::string(), primPath, layerOffset);
  return AddReference(reference, position);
}

bool UsdReferences::RemoveReference(const SdfReference &refIn)
{
  return _ListEditImpl::Remove(*this, refIn);
}

bool UsdReferences::ClearReferences()
{
  return _ListEditImpl::Clear(*this);
}

bool UsdReferences::SetReferences(const SdfReferenceVector &itemsIn)
{
  return _ListEditImpl::Set(*this, itemsIn);
}

WABI_NAMESPACE_END
