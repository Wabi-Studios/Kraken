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
#ifndef WABI_IMAGING_PLUGIN_HD_EMBREE_RENDER_PASS_H
#define WABI_IMAGING_PLUGIN_HD_EMBREE_RENDER_PASS_H

#include "wabi/wabi.h"

#include "wabi/imaging/hd/aov.h"
#include "wabi/imaging/hd/renderPass.h"
#include "wabi/imaging/hd/renderThread.h"
#include "wabi/imaging/plugin/hdEmbree/renderBuffer.h"
#include "wabi/imaging/plugin/hdEmbree/renderer.h"

#include "wabi/base/gf/matrix4d.h"
#include "wabi/base/gf/rect2i.h"

#include <atomic>

WABI_NAMESPACE_BEGIN

/// \class HdEmbreeRenderPass
///
/// HdRenderPass represents a single render iteration, rendering a view of the
/// scene (the HdRprimCollection) for a specific viewer (the camera/viewport
/// parameters in HdRenderPassState) to the current draw target.
///
/// This class does so by raycasting into the embree scene via HdEmbreeRenderer.
///
class HdEmbreeRenderPass final : public HdRenderPass
{
 public:

  /// Renderpass constructor.
  ///   \param index The render index containing scene data to render.
  ///   \param collection The initial rprim collection for this renderpass.
  ///   \param renderThread A handle to the global render thread.
  ///   \param renderer A handle to the global renderer.
  HdEmbreeRenderPass(HdRenderIndex *index,
                     HdRprimCollection const &collection,
                     HdRenderThread *renderThread,
                     HdEmbreeRenderer *renderer,
                     std::atomic<int> *sceneVersion);

  /// Renderpass destructor.
  ~HdEmbreeRenderPass() override;

  // -----------------------------------------------------------------------
  // HdRenderPass API

  /// Determine whether the sample buffer has enough samples.
  ///   \return True if the image has enough samples to be considered final.
  bool IsConverged() const override;

 protected:

  // -----------------------------------------------------------------------
  // HdRenderPass API

  /// Draw the scene with the bound renderpass state.
  ///   \param renderPassState Input parameters (including viewer parameters)
  ///                          for this renderpass.
  ///   \param renderTags Which rendertags should be drawn this pass.
  void _Execute(HdRenderPassStateSharedPtr const &renderPassState,
                TfTokenVector const &renderTags) override;

  /// Update internal tracking to reflect a dirty collection.
  void _MarkCollectionDirty() override {}

 private:

  // A handle to the render thread.
  HdRenderThread *_renderThread;

  // A handle to the global renderer.
  HdEmbreeRenderer *_renderer;

  // A reference to the global scene version.
  std::atomic<int> *_sceneVersion;

  // The last scene version we rendered with.
  int _lastSceneVersion;

  // The last settings version we rendered with.
  int _lastSettingsVersion;

  // The pixels written to. Like viewport in OpenGL,
  // but coordinates are y-Down.
  GfRect2i _dataWindow;

  // The view matrix: world space to camera space
  GfMatrix4d _viewMatrix;
  // The projection matrix: camera space to NDC space (with
  // respect to the data window).
  GfMatrix4d _projMatrix;

  // The list of aov buffers this renderpass should write to.
  HdRenderPassAovBindingVector _aovBindings;

  // If no attachments are provided, provide an anonymous renderbuffer for
  // color and depth output.
  HdEmbreeRenderBuffer _colorBuffer;
  HdEmbreeRenderBuffer _depthBuffer;

  // Were the color/depth buffer converged the last time we blitted them?
  bool _converged;
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_PLUGIN_HD_EMBREE_RENDER_PASS_H
