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
#ifndef WABI_IMAGING_HD_EXT_COMP_CPU_COMPUTATION_H
#define WABI_IMAGING_HD_EXT_COMP_CPU_COMPUTATION_H

#include "wabi/imaging/hd/api.h"
#include "wabi/imaging/hd/extCompInputSource.h"
#include "wabi/wabi.h"

#include "wabi/usd/sdf/path.h"

#include "wabi/base/tf/token.h"
#include "wabi/base/vt/value.h"

#include <memory>

WABI_NAMESPACE_BEGIN

class HdSceneDelegate;
class HdExtComputation;
class HdExtCompCpuComputation;

using HdExtCompCpuComputationSharedPtr = std::shared_ptr<HdExtCompCpuComputation>;

///
/// A Buffer Source that represents a CPU implementation of a ExtComputation.
///
/// The computation implements the basic: input->processing->output model
/// where the inputs are other buffer sources and processing happens during
/// resolve.
///
/// As a computation may have many outputs, the outputs from the CPU
/// Computation can not be directly associated with a BAR.  Instead
/// other buffer source computation bind the output to sources that can
/// be used in a BAR.
///
/// Outputs to a computation are in SOA form, so a computation may have
/// many outputs, but each output has the same number of elements in it.
class HdExtCompCpuComputation final : public HdNullBufferSource
{
 public:
  static const size_t INVALID_OUTPUT_INDEX;

  /// Constructs a new Cpu ExtComputation source.
  /// inputs provides a list of buffer sources that this computation
  /// requires.
  /// outputs is a list of outputs by names that the computation produces.
  ///
  /// Num elements specifies the number of elements in the output.
  ///
  /// sceneDelegate and id are used to callback to the scene delegate
  /// in order to invoke computation processing.
  HdExtCompCpuComputation(const SdfPath &id,
                          const Hd_ExtCompInputSourceSharedPtrVector &inputs,
                          const TfTokenVector &outputs,
                          int numElements,
                          HdSceneDelegate *sceneDelegate);

  /// Create a CPU computation implementing the given abstract computation.
  /// The scene delegate identifies which delegate to pull scene inputs from.
  HD_API
  static HdExtCompCpuComputationSharedPtr CreateComputation(
    HdSceneDelegate *sceneDelegate,
    const HdExtComputation &computation,
    HdBufferSourceSharedPtrVector *computationSources);

  HD_API
  virtual ~HdExtCompCpuComputation() = default;

  /// Returns the id for this computation as a token.
  HD_API
  virtual TfToken const &GetName() const override;

  /// Ask the scene delegate to run the computation and captures the output
  /// signals.
  HD_API
  virtual bool Resolve() override;

  HD_API
  virtual size_t GetNumElements() const override;

  /// Converts a output name token into an index.
  HD_API
  size_t GetOutputIndex(const TfToken &outputName) const;

  /// Returns the value of the specified output
  /// (after the computations been Resolved).
  HD_API
  const VtValue &GetOutputByIndex(size_t index) const;

 protected:
  /// Returns if the computation is specified correctly.
  HD_API
  virtual bool _CheckValid() const override;

 private:
  SdfPath _id;
  Hd_ExtCompInputSourceSharedPtrVector _inputs;
  TfTokenVector _outputs;
  size_t _numElements;
  HdSceneDelegate *_sceneDelegate;

  std::vector<VtValue> _outputValues;

  HdExtCompCpuComputation() = delete;
  HdExtCompCpuComputation(const HdExtCompCpuComputation &) = delete;
  HdExtCompCpuComputation &operator=(const HdExtCompCpuComputation &) = delete;
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HD_EXT_COMP_CPU_COMPUTATION_H
