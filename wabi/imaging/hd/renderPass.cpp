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
#include "wabi/imaging/hd/renderPass.h"

#include "wabi/imaging/hd/changeTracker.h"
#include "wabi/imaging/hd/dirtyList.h"
#include "wabi/imaging/hd/renderIndex.h"
#include "wabi/imaging/hd/renderPassState.h"
#include "wabi/imaging/hd/sceneDelegate.h"
#include "wabi/imaging/hd/tokens.h"

#include "wabi/base/tf/staticTokens.h"
#include "wabi/base/tf/stringUtils.h"

WABI_NAMESPACE_BEGIN

HdRenderPass::HdRenderPass(HdRenderIndex *index, HdRprimCollection const &collection)
    : _renderIndex(index)
{
  SetRprimCollection(collection);
}

HdRenderPass::~HdRenderPass()
{
  /*NOTHING*/
}

void HdRenderPass::SetRprimCollection(HdRprimCollection const &col)
{
  if (col == _collection) {
    return;
  }

  _collection = col;

  // update dirty list subscription for the new collection.
  // holding shared_ptr for the lifetime of the dirty list.
  bool isMinorChange = true;
  if (!_dirtyList || !_dirtyList->ApplyEdit(col)) {
    _dirtyList.reset(new HdDirtyList(_collection, *_renderIndex));
    isMinorChange = false;
  }

  if (TfDebug::IsEnabled(HD_DIRTY_LIST)) {
    std::stringstream s;
    s << "  Include: \n";
    for (auto i : col.GetRootPaths()) {
      s << "    - " << i << "\n";
    }
    s << "  Exclude: \n";
    for (auto i : col.GetExcludePaths()) {
      s << "    - " << i << "\n";
    }
    s << "  Repr: " << col.GetReprSelector() << "\n";

    TF_DEBUG(HD_DIRTY_LIST)
        .Msg(
            "RenderPass(%p)::SetRprimCollection (%s) - "
            "constructing new DirtyList(%p) minorChange(%d) \n%s\n",
            (void *)this,
            col.GetName().GetText(),
            (void *)&*_dirtyList,
            isMinorChange,
            s.str().c_str());
  }

  // Mark the collection dirty in derived classes.
  _MarkCollectionDirty();
}

void HdRenderPass::Execute(HdRenderPassStateSharedPtr const &renderPassState,
                           TfTokenVector const &renderTags)
{
  _Execute(renderPassState, renderTags);
}

void HdRenderPass::Sync()
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  // Enqueue the dirty list of prims to be synced during Hydra Sync.
  _renderIndex->EnqueuePrimsToSync(_dirtyList, _collection);

  // Give derived classes a chance to sync.
  _Sync();
}

void HdRenderPass::Prepare(TfTokenVector const &renderTags)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  _Prepare(renderTags);
}

WABI_NAMESPACE_END
