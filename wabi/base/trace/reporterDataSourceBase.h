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

#ifndef WABI_BASE_TRACE_REPORTER_DATA_SOURCE_BASE_H
#define WABI_BASE_TRACE_REPORTER_DATA_SOURCE_BASE_H

#include "wabi/wabi.h"

#include "wabi/base/trace/api.h"
#include "wabi/base/trace/collection.h"

#include <vector>

WABI_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
/// \class TraceReporterDataSourceBase
///
/// This class is a base class for TraceReporterBase data sources.
/// TraceReporterBase uses an instance of a TraceReporterDataSourceBase derived
/// class to access TraceCollections.
///
class TraceReporterDataSourceBase
{
 public:

  using CollectionPtr = std::shared_ptr<TraceCollection>;

  /// Destructor
  TRACE_API virtual ~TraceReporterDataSourceBase();

  /// Removes all references to TraceCollections.
  virtual void Clear() = 0;

  /// Returns the next TraceCollections which need to be processed.
  virtual std::vector<CollectionPtr> ConsumeData() = 0;
};

WABI_NAMESPACE_END

#endif  // WABI_BASE_TRACE_REPORTER_DATA_SOURCE_BASE_H