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
#include "wabi/usd/usd/attributeQuery.h"
#include "wabi/wabi.h"

#include "wabi/base/tf/makePyConstructor.h"
#include "wabi/base/tf/pyContainerConversions.h"
#include "wabi/base/tf/pyPtrHelpers.h"
#include "wabi/base/tf/pyResultConversions.h"
#include "wabi/usd/usd/pyConversions.h"

#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/tuple.hpp>

#include <string>
#include <vector>

using std::string;
using std::vector;

using namespace boost::python;

WABI_NAMESPACE_USING

namespace {

static vector<double> _GetTimeSamples(const UsdAttributeQuery &query)
{
  vector<double> result;
  query.GetTimeSamples(&result);
  return result;
}

static vector<double> _GetTimeSamplesInInterval(const UsdAttributeQuery &query,
                                                const GfInterval &interval)
{
  vector<double> result;
  query.GetTimeSamplesInInterval(interval, &result);
  return result;
}

static vector<double> _GetUnionedTimeSamples(const vector<UsdAttributeQuery> &attrQueries)
{
  vector<double> result;
  UsdAttributeQuery::GetUnionedTimeSamples(attrQueries, &result);
  return result;
}

static vector<double> _GetUnionedTimeSamplesInInterval(
    const vector<UsdAttributeQuery> &attrQueries,
    const GfInterval &interval)
{
  vector<double> result;
  UsdAttributeQuery::GetUnionedTimeSamplesInInterval(attrQueries, interval, &result);
  return result;
}

static object _GetBracketingTimeSamples(const UsdAttributeQuery &self, double desiredTime)
{
  double lower = 0.0, upper = 0.0;
  bool hasTimeSamples = false;

  if (self.GetBracketingTimeSamples(desiredTime, &lower, &upper, &hasTimeSamples)) {
    return hasTimeSamples ? make_tuple(lower, upper) : make_tuple();
  }
  return object();
}

static TfPyObjWrapper _Get(const UsdAttributeQuery &self, UsdTimeCode time)
{
  VtValue val;
  self.Get(&val, time);
  return UsdVtValueToPython(val);
}

}  // anonymous namespace

void wrapUsdAttributeQuery()
{
  class_<UsdAttributeQuery, boost::noncopyable>("AttributeQuery", no_init)
      .def(init<const UsdAttribute &>((arg("attribute"))))

      .def(init<const UsdPrim &, const TfToken &>((arg("prim"), arg("attributeName"))))

      .def("CreateQueries",
           &UsdAttributeQuery::CreateQueries,
           (arg("prim"), arg("attributeNames")),
           return_value_policy<TfPySequenceToList>())
      .staticmethod("CreateQueries")

      .def("IsValid", &UsdAttributeQuery::IsValid)
      .def(!self)

      .def(
          "GetAttribute", &UsdAttributeQuery::GetAttribute, return_value_policy<return_by_value>())

      .def("GetTimeSamples", _GetTimeSamples, return_value_policy<TfPySequenceToList>())

      .def("GetTimeSamplesInInterval",
           _GetTimeSamplesInInterval,
           arg("interval"),
           return_value_policy<TfPySequenceToList>())

      .def("GetUnionedTimeSamples",
           _GetUnionedTimeSamples,
           arg("attrQueries"),
           return_value_policy<TfPySequenceToList>())
      .staticmethod("GetUnionedTimeSamples")

      .def("GetUnionedTimeSamplesInInterval",
           _GetUnionedTimeSamplesInInterval,
           (arg("attrQueries"), arg("interval")),
           return_value_policy<TfPySequenceToList>())
      .staticmethod("GetUnionedTimeSamplesInInterval")

      .def("GetNumTimeSamples", &UsdAttributeQuery::GetNumTimeSamples)
      .def("GetBracketingTimeSamples", _GetBracketingTimeSamples, arg("desiredTime"))

      .def("HasValue", &UsdAttributeQuery::HasValue)
      .def("HasAuthoredValueOpinion", &UsdAttributeQuery::HasAuthoredValueOpinion)
      .def("HasAuthoredValue", &UsdAttributeQuery::HasAuthoredValue)
      .def("HasFallbackValue", &UsdAttributeQuery::HasFallbackValue)

      .def("ValueMightBeTimeVarying", &UsdAttributeQuery::ValueMightBeTimeVarying)

      .def("Get", _Get, arg("time") = UsdTimeCode::Default())

      ;

  TfPyRegisterStlSequencesFromPython<UsdAttributeQuery>();
}
