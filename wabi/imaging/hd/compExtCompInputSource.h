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

#ifndef WABI_IMAGING_HD_COMP_EXT_COMP_INPUT_SOURCE_H
#define WABI_IMAGING_HD_COMP_EXT_COMP_INPUT_SOURCE_H

#include "wabi/imaging/hd/api.h"
#include "wabi/imaging/hd/extCompInputSource.h"
#include "wabi/imaging/hd/version.h"
#include "wabi/wabi.h"

#include <memory>

WABI_NAMESPACE_BEGIN

class HdExtCompCpuComputation;

using HdExtCompCpuComputationSharedPtr = std::shared_ptr<HdExtCompCpuComputation>;

///
/// An Hd Buffer Source Computation that is used to bind an ExtComputation input
/// to a specific output of another ExtComputation.
///
class Hd_CompExtCompInputSource final : public Hd_ExtCompInputSource
{
 public:
  /// Constructs the computation, binding inputName to sourceOutputName
  /// on buffer source representation of the source computation.
  HD_API
  Hd_CompExtCompInputSource(const TfToken &inputName,
                            const HdExtCompCpuComputationSharedPtr &source,
                            const TfToken &sourceOutputName);

  HD_API
  virtual ~Hd_CompExtCompInputSource() = default;

  /// Returns true once the source computation has been resolved.
  HD_API
  virtual bool Resolve() override;

  /// Obtains the value of the output from the source computation.
  HD_API
  virtual const VtValue &GetValue() const override;

 protected:
  /// Returns true if the binding is successful.
  virtual bool _CheckValid() const override;

 private:
  HdExtCompCpuComputationSharedPtr _source;
  size_t _sourceOutputIdx;

  Hd_CompExtCompInputSource() = delete;
  Hd_CompExtCompInputSource(const Hd_CompExtCompInputSource &) = delete;
  Hd_CompExtCompInputSource &operator=(const Hd_CompExtCompInputSource &) = delete;
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HD_COMP_EXT_COMP_INPUT_SOURCE_H
