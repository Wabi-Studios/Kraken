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
#include "wabi/usd/usdSkel/binding.h"

#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyPtrHelpers.h"
#include "wabi/base/tf/pyResultConversions.h"
#include "wabi/base/tf/pyUtils.h"
#include "wabi/base/tf/wrapTypeHelpers.h"

#include "wabi/usd/usd/pyConversions.h"

#include "wabi/usd/usdSkel/cache.h"
#include "wabi/usd/usdSkel/root.h"

#include <boost/python.hpp>

using namespace boost::python;

WABI_NAMESPACE_USING

namespace {

UsdSkelBinding *_New(const UsdSkelSkeleton &skel, const boost::python::list &skinningQueries)
{
  const size_t numQueries = len(skinningQueries);
  VtArray<UsdSkelSkinningQuery> skinningQueriesArray(numQueries);
  for (size_t i = 0; i < numQueries; ++i) {
    skinningQueriesArray[i] = extract<const UsdSkelSkinningQuery &>(skinningQueries[i]);
  }
  return new UsdSkelBinding(skel, skinningQueriesArray);
}

}  // namespace

void wrapUsdSkelBinding()
{
  using This = UsdSkelBinding;

  class_<This>("Binding", init<>())

    .def("__init__", make_constructor(&_New))

    .def("GetSkeleton", &This::GetSkeleton, return_value_policy<return_by_value>())

    .def("GetSkinningTargets", &This::GetSkinningTargets, return_value_policy<TfPySequenceToList>());
}
