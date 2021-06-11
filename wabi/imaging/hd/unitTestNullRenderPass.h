//
// Copyright 2016 Pixar
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
#ifndef WABI_IMAGING_HD_UNIT_TEST_NULL_RENDER_PASS_H
#define WABI_IMAGING_HD_UNIT_TEST_NULL_RENDER_PASS_H

#include "wabi/imaging/hd/api.h"
#include "wabi/imaging/hd/version.h"
#include "wabi/wabi.h"

#include "wabi/imaging/hd/renderPass.h"

WABI_NAMESPACE_BEGIN

///
/// \class Hd_UnitTestNullRenderPass
/// Implements the sync part of the render pass, but not the draw part, for
/// core hydra unit tests.
class Hd_UnitTestNullRenderPass : public HdRenderPass {
 public:
  Hd_UnitTestNullRenderPass(HdRenderIndex *index, HdRprimCollection const &collection)
      : HdRenderPass(index, collection)
  {}
  virtual ~Hd_UnitTestNullRenderPass()
  {}

  void _Execute(HdRenderPassStateSharedPtr const &renderPassState,
                TfTokenVector const &renderTags) override
  {}
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HD_UNIT_TEST_NULL_RENDER_PASS_H
