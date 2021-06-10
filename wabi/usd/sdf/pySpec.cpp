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
/// \file PySpec.cpp

#include "wabi/usd/sdf/pySpec.h"
#include "wabi/base/arch/demangle.h"
#include "wabi/base/tf/diagnostic.h"
#include "wabi/base/tf/pyUtils.h"
#include "wabi/base/tf/staticData.h"
#include "wabi/base/tf/type.h"
#include "wabi/usd/sdf/layer.h"
#include "wabi/usd/sdf/spec.h"
#include "wabi/usd/sdf/specType.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

namespace Sdf_PySpecDetail {

bp::object _DummyInit(bp::tuple const & /* args */, bp::dict const & /* kw */)
{
  return bp::object();
}

// Returns a repr based on Sdf.Find().
std::string _SpecRepr(const bp::object &self, const SdfSpec *spec)
{
  if (!spec || spec->IsDormant() || !spec->GetLayer()) {
    return "<dormant " + TfPyGetClassName(self) + ">";
  }
  else {
    SdfLayerHandle layer = spec->GetLayer();
    std::string path     = layer->GetIdentifier();
    return TF_PY_REPR_PREFIX + "Find(" + TfPyRepr(path) + ", " +
           TfPyRepr(spec->GetPath().GetString()) + ")";
  }
}

typedef std::map<TfType, _HolderCreator> _HolderCreatorMap;
static TfStaticData<_HolderCreatorMap> _holderCreators;

void _RegisterHolderCreator(const std::type_info &ti, _HolderCreator creator)
{
  TfType type = TfType::Find(ti);
  if (type.IsUnknown()) {
    TF_CODING_ERROR("No TfType registered for type \"%s\"", ArchGetDemangled(ti).c_str());
  }
  else if (!_holderCreators->insert(std::make_pair(type, creator)).second) {
    TF_CODING_ERROR("Duplicate conversion for \"%s\" ignored", type.GetTypeName().c_str());
  }
}

PyObject *_CreateHolder(const std::type_info &ti, const SdfSpec &spec)
{
  if (spec.IsDormant()) {
    return bp::detail::none();
  }
  else {
    // Get the TfType for the object's actual type.  If there's an
    // ambiguity (e.g. for SdfVariantSpec) then use type ti.
    TfType type = Sdf_SpecType::Cast(spec, ti);

    // Find the creator for the type and invoke it.
    _HolderCreatorMap::const_iterator i = _holderCreators->find(type);
    if (i == _holderCreators->end()) {
      if (!type.IsUnknown()) {
        TF_CODING_ERROR("No conversion for registed for \"%s\"", type.GetTypeName().c_str());
      }
      return bp::detail::none();
    }
    else {
      return (i->second)(spec);
    }
  }
}

}  // namespace Sdf_PySpecDetail

WABI_NAMESPACE_END
