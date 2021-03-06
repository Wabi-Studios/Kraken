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
#include "wabi/imaging/hdSt/drawTargetRenderPassState.h"
#include "wabi/base/vt/value.h"
#include "wabi/imaging/hd/aov.h"

WABI_NAMESPACE_BEGIN


HdStDrawTargetRenderPassState::HdStDrawTargetRenderPassState()
 : _depthPriority(HdDepthPriorityNearest)
 , _cameraId()
 , _rprimCollection()
 , _rprimCollectionVersion(1) // Clients start at 0
{
}

HdStDrawTargetRenderPassState::~HdStDrawTargetRenderPassState() = default;

void
HdStDrawTargetRenderPassState::SetDepthPriority(HdDepthPriority priority)
{
    _depthPriority = priority;
}

void
HdStDrawTargetRenderPassState::SetCamera(const SdfPath &cameraId)
{
    _cameraId = cameraId;
}


void HdStDrawTargetRenderPassState::SetRprimCollection(
                                                   HdRprimCollection const& col)
{
    _rprimCollection = col;
    ++_rprimCollectionVersion;
}

void HdStDrawTargetRenderPassState::SetAovBindings(
    const HdRenderPassAovBindingVector &aovBindings)
{
    _aovBindings = aovBindings;
}

WABI_NAMESPACE_END

