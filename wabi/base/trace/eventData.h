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

#ifndef WABI_BASE_TRACE_EVENT_DATA_H
#define WABI_BASE_TRACE_EVENT_DATA_H

#include "wabi/wabi.h"

#include "wabi/base/trace/api.h"
#include "wabi/base/trace/event.h"

#include <boost/variant.hpp>
#include <string>

WABI_NAMESPACE_BEGIN

class JsWriter;
////////////////////////////////////////////////////////////////////////////////
///
/// \class TraceEventData
///
/// This class holds data that can be stored in TraceEvents.
///
class TraceEventData
{
 public:

  /// Ctor for Invalid type.
  TraceEventData() : _data(_NoData()) {}

  /// Ctor for Bool type.
  explicit TraceEventData(bool b) : _data(b) {}

  /// Ctor for Int type.
  explicit TraceEventData(int64_t i) : _data(i) {}

  /// Ctor for UInt type.
  explicit TraceEventData(uint64_t i) : _data(i) {}

  /// Ctor for Float type.
  explicit TraceEventData(double d) : _data(d) {}

  /// Ctor for String type.
  explicit TraceEventData(const std::string &s) : _data(s) {}

  /// Returns the Type of the data stored.
  TRACE_API TraceEvent::DataType GetType() const;

  /// Returns a pointer to the data or nullptr if the type is not Int.
  TRACE_API const int64_t *GetInt() const;

  /// Returns a pointer to the data or nullptr if the type is not UInt.
  TRACE_API const uint64_t *GetUInt() const;

  /// Returns a pointer to the data or nullptr if the type is not Float.
  TRACE_API const double *GetFloat() const;

  /// Returns a pointer to the data or nullptr if the type is not Bool.
  TRACE_API const bool *GetBool() const;

  /// Returns a pointer to the data or nullptr if the type is not String.
  TRACE_API const std::string *GetString() const;

  /// Writes a json representation of the data.
  TRACE_API void WriteJson(JsWriter &) const;

 private:

  // Type that represents no data was stored in an event.
  struct _NoData
  {};

  using Variant = boost::variant<_NoData, std::string, bool, int64_t, uint64_t, double>;
  Variant _data;
};

WABI_NAMESPACE_END

#endif  // WABI_BASE_TRACE_EVENT_DATA_H