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
#include "wabi/base/trace/serialization.h"

#include "wabi/base/trace/jsonSerialization.h"
#include "wabi/wabi.h"

#include "wabi/base/js/json.h"
#include "wabi/base/tf/scopeDescription.h"

WABI_NAMESPACE_BEGIN

bool TraceSerialization::Write(std::ostream &ostr,
                               const std::shared_ptr<TraceCollection> &collection)
{
  if (!collection) {
    return false;
  }
  return Write(ostr, std::vector<std::shared_ptr<TraceCollection>>{collection});
}

bool TraceSerialization::Write(std::ostream &ostr,
                               const std::vector<std::shared_ptr<TraceCollection>> &collections)
{
  JsValue colVal;
  if (collections.empty()) {
    return false;
  }
  {
    TF_DESCRIBE_SCOPE("Writing JSON");
    JsWriter js(ostr);
    Trace_JSONSerialization::WriteCollectionsToJSON(js, collections);
    return true;
  }
  return false;
}

std::unique_ptr<TraceCollection> TraceSerialization::Read(std::istream &istr,
                                                          std::string *errorStr)
{
  JsParseError error;
  JsValue value = JsParseStream(istr, &error);
  if (value.IsNull()) {
    if (errorStr) {
      *errorStr = TfStringPrintf(
        "Error parsing JSON\n"
        "line: %d, col: %d ->\n\t%s.\n",
        error.line,
        error.column,
        error.reason.c_str());
    }
    return nullptr;
  }
  return Trace_JSONSerialization::CollectionFromJSON(value);
}

WABI_NAMESPACE_END
