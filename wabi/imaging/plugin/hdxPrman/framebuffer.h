//
// Copyright 2019 Pixar
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
#ifndef EXT_RMANPKG_23_0_PLUGIN_RENDERMAN_PLUGIN_HDX_PRMAN_FRAMEBUFFER_H
#define EXT_RMANPKG_23_0_PLUGIN_RENDERMAN_PLUGIN_HDX_PRMAN_FRAMEBUFFER_H

#include "wabi/imaging/hd/types.h"
#include "wabi/wabi.h"

#include "wabi/base/gf/matrix4d.h"

#include "Riley.h"

#include <mutex>
#include <vector>

class RixContext;

WABI_NAMESPACE_BEGIN

/// A simple framebuffer used to receive display-driver output from PRMan.
/// This lives in a separate small library so it can be accessible to
/// both the hdPrman hydra plgin at the d_hydra display driver plugin,
/// without requiring either to know about the other.
class HdxPrmanFramebuffer
{
 public:

  struct HdPrmanAov
  {
    TfToken name;
    HdFormat format;
    VtValue clearValue;
    std::vector<uint32_t> pixels;
  };
  typedef std::vector<HdPrmanAov> HdPrmanAovList;
  typedef std::vector<HdPrmanAov>::iterator HdPrmanAovIt;

  HdxPrmanFramebuffer();
  ~HdxPrmanFramebuffer();

  /// Find a buffer instance with the given ID.
  /// The expectation is that the buffer will exist, so
  /// this raises a runtime error if the ID is not found.
  static HdxPrmanFramebuffer *GetByID(int32_t id);
  static void Register(RixContext *);

  /// Resize the buffer.
  void Resize(int width, int height);

  void Clear();

  void AddAov(TfToken aovName, HdFormat dataType, VtValue clearValue);

  std::mutex mutex;
  HdPrmanAovList aovs;

  int w, h;
  int32_t id;

  // Projection matrix (for the depth output).
  GfMatrix4d proj;

  // Clear functionality.
  bool pendingClear;

  riley::DisplayId dspyId;
  riley::RenderTargetId rtId;
};

WABI_NAMESPACE_END

#endif
