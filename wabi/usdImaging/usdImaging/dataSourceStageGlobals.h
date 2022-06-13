//
// Copyright 2020 Pixar
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
#ifndef WABI_USD_IMAGING_USD_IMAGING_DATA_SOURCE_STAGE_GLOBALS_H
#define WABI_USD_IMAGING_USD_IMAGING_DATA_SOURCE_STAGE_GLOBALS_H

#include "wabi/usd/usd/prim.h"
#include "wabi/usd/usd/timeCode.h"

#include "wabi/imaging/hd/dataSourceLocator.h"

#include "wabi/usd/sdf/path.h"

WABI_NAMESPACE_BEGIN

/// \class UsdImagingDataSourceStageGlobals
///
/// This class is used as a context object with global stage information,
/// that gets passed down to datasources to help them answer scene queries.
///
/// It's a pure virtual interface to allow for different use cases to override
/// certain behaviors (like getting the time coordinate, or whether we support
/// time-varying tracking).
///
class UsdImagingDataSourceStageGlobals
{
 public:

  virtual ~UsdImagingDataSourceStageGlobals();

  // Datasource API

  /// Returns the current time represented in this instance.
  virtual UsdTimeCode GetTime() const = 0;

  /// Flags the given \p primPath as time varying at the given locator.
  virtual void FlagAsTimeVarying(const SdfPath &primPath,
                                 const HdDataSourceLocator &locator) const = 0;
};


WABI_NAMESPACE_END

#endif  // WABI_USD_IMAGING_USD_IMAGING_DATA_SOURCE_STAGE_GLOBALS_H
