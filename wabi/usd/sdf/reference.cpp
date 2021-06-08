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
// Reference.cpp
//

#include "wabi/usd/sdf/reference.h"
#include "wabi/wabi.h"

#include "wabi/base/tf/registryManager.h"
#include "wabi/base/tf/type.h"

#include <algorithm>
#include <functional>
#include <ostream>

WABI_NAMESPACE_BEGIN

TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<SdfReference>();
  TfType::Define<SdfReferenceVector>();
}

SdfReference::SdfReference(const std::string &assetPath,
                           const SdfPath &primPath,
                           const SdfLayerOffset &layerOffset,
                           const VtDictionary &customData)
    : _assetPath(assetPath),
      _primPath(primPath),
      _layerOffset(layerOffset),
      _customData(customData)
{}

void SdfReference::SetCustomData(const std::string &name, const VtValue &value)
{
  if (value.IsEmpty()) {
    _customData.erase(name);
  }
  else {
    _customData[name] = value;
  }
}

bool SdfReference::IsInternal() const
{
  return _assetPath.empty();
}

bool SdfReference::operator==(const SdfReference &rhs) const
{
  return _assetPath == rhs._assetPath && _primPath == rhs._primPath &&
         _layerOffset == rhs._layerOffset && _customData == rhs._customData;
}

bool SdfReference::operator<(const SdfReference &rhs) const
{
  // XXX: This would be much cleaner and less error prone if we used
  // std::tie for comparison, however, it's not ideal given the awkward
  // (and not truly correct) comparison of customData size. If customData
  // is ever removed this should be updated to use the cleaner std::tie.
  return (_assetPath < rhs._assetPath ||
          (_assetPath == rhs._assetPath &&
           (_primPath < rhs._primPath ||
            (_primPath == rhs._primPath && (_layerOffset < rhs._layerOffset ||
                                            (_layerOffset == rhs._layerOffset &&
                                             (_customData.size() < rhs._customData.size())))))));
}

int SdfFindReferenceByIdentity(const SdfReferenceVector &references,
                               const SdfReference &referenceId)
{
  SdfReferenceVector::const_iterator it = std::find_if(
      references.begin(), references.end(), [&referenceId](SdfReference const &ref) {
        return SdfReference::IdentityEqual()(referenceId, ref);
      });

  return it != references.end() ? it - references.begin() : -1;
}

std::ostream &operator<<(std::ostream &out, const SdfReference &reference)
{
  return out << "SdfReference(" << reference.GetAssetPath() << ", " << reference.GetPrimPath()
             << ", " << reference.GetLayerOffset() << ", " << reference.GetCustomData() << ")";
}

WABI_NAMESPACE_END
