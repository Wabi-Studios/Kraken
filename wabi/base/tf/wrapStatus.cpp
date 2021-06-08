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

#include "wabi/base/tf/diagnosticMgr.h"
#include "wabi/base/tf/pyCallContext.h"
#include "wabi/base/tf/status.h"
#include "wabi/base/tf/stringUtils.h"

#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/scope.hpp>

using std::string;

using namespace boost::python;

WABI_NAMESPACE_USING

namespace {

static void _Status(string const &msg,
                    string const &moduleName,
                    string const &functionName,
                    string const &fileName,
                    int lineNo)
{
  TfDiagnosticMgr::StatusHelper(
      Tf_PythonCallContext(fileName.c_str(), moduleName.c_str(), functionName.c_str(), lineNo),
      TF_DIAGNOSTIC_STATUS_TYPE,
      TfEnum::GetName(TfEnum(TF_DIAGNOSTIC_STATUS_TYPE)).c_str())
      .Post(msg);
}

static string TfStatus__repr__(TfStatus const &self)
{
  string ret = TfStringPrintf("Status in '%s' at line %zu in file %s : '%s'",
                              self.GetSourceFunction().c_str(),
                              self.GetSourceLineNumber(),
                              self.GetSourceFileName().c_str(),
                              self.GetCommentary().c_str());

  return ret;
}

}  // anonymous namespace

void wrapStatus()
{
  def("_Status", &_Status);

  typedef TfStatus This;

  // Can't call this scope Status because Tf.Status() is a function def'd
  // above.
  scope statusScope = class_<This, bases<TfDiagnosticBase>>("StatusObject", no_init)

                          .def("__repr__", TfStatus__repr__);
}
