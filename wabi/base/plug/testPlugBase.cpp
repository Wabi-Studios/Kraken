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

#include "wabi/wabi.h"
#include "wabi/base/plug/testPlugBase.h"
#include "wabi/base/plug/registry.h"
#include "wabi/base/tf/diagnostic.h"
#include "wabi/base/tf/type.h"
#include "wabi/base/tf/stringUtils.h"
#include "wabi/base/tf/type.h"

WABI_NAMESPACE_BEGIN

template<int N>
TfRefPtr<_TestPlugBase<N>> _TestPlugBase<N>::Manufacture(const std::string &subclass)
{
  // Lookup TfType for subclass
  const TfType &t = PlugRegistry::FindTypeByName(subclass);
  if (t.IsUnknown()) {
    TF_CODING_ERROR("Failed to find TfType for %s", subclass.c_str());
    return TfNullPtr;
  }

  // Manufacture an instance.
  if (_TestPlugFactoryBase<N> *factory = t.GetFactory<_TestPlugFactoryBase<N>>()) {
    return factory->New();
  }

  return TfNullPtr;
}

// XXX -- These shouldn't be in the library or used by the Python module.
template class _TestPlugBase<1>;
template class _TestPlugBase<2>;
template class _TestPlugBase<3>;
template class _TestPlugBase<4>;

// This derived class should be discovered as an available subclass
// of _TestPlugBase1 even though it is compiled into the base library.
class _TestPlugDerived0 : public _TestPlugBase1
{
 public:

  typedef _TestPlugDerived0 This;
  typedef TfRefPtr<This> RefPtr;
  typedef TfWeakPtr<This> Ptr;

  virtual ~_TestPlugDerived0() {}

  // Return our base type, since this class is not wrapped for Python.
  static TfRefPtr<_TestPlugBase1> New()
  {
    return TfCreateRefPtr(new This());
  }

 protected:

  _TestPlugDerived0() {}
};

TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<_TestPlugBase1>().SetFactory<_TestPlugFactory<_TestPlugBase1>>();
  TfType::Define<_TestPlugBase2>().SetFactory<_TestPlugFactory<_TestPlugBase2>>();
  TfType::Define<_TestPlugBase3>().SetFactory<_TestPlugFactory<_TestPlugBase3>>();
  TfType::Define<_TestPlugBase4>().SetFactory<_TestPlugFactory<_TestPlugBase4>>();

  TfType::Define<_TestPlugDerived0, TfType::Bases<_TestPlugBase1>>()
    .SetFactory<_TestPlugFactory<_TestPlugDerived0>>();
}

WABI_NAMESPACE_END
