//
// Copyright 2017 Pixar
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

#ifndef WABI_IMAGING_HD_EXT_COMP_INPUT_SOURCE_H
#define WABI_IMAGING_HD_EXT_COMP_INPUT_SOURCE_H

#include "wabi/wabi.h"
#include "wabi/imaging/hd/api.h"
#include "wabi/imaging/hd/version.h"
#include "wabi/imaging/hd/bufferSource.h"

#include <memory>

WABI_NAMESPACE_BEGIN

class VtValue;

///
/// Abstract base class for a Buffer Source that represents a binding to an
/// input to an ExtComputation.
///
///
class Hd_ExtCompInputSource : public HdNullBufferSource
{
 public:

  /// Constructs the input binding with the name inputName
  HD_API
  Hd_ExtCompInputSource(const TfToken &inputName);

  HD_API
  virtual ~Hd_ExtCompInputSource() = default;

  /// Returns the name of the input.
  HD_API
  virtual TfToken const &GetName() const final;

  /// Returns the value associated with the input.
  HD_API
  virtual const VtValue &GetValue() const = 0;

 private:

  TfToken _inputName;

  Hd_ExtCompInputSource() = delete;
  Hd_ExtCompInputSource(const Hd_ExtCompInputSource &) = delete;
  Hd_ExtCompInputSource &operator=(const Hd_ExtCompInputSource &) = delete;
};

using Hd_ExtCompInputSourceSharedPtr = std::shared_ptr<Hd_ExtCompInputSource>;
using Hd_ExtCompInputSourceSharedPtrVector = std::vector<Hd_ExtCompInputSourceSharedPtr>;


WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HD_EXT_COMP_INPUT_SOURCE_H
