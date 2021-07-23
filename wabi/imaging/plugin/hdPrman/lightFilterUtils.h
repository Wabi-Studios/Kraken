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
#ifndef EXT_RMANPKG_22_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_MATFILT_CONVERSIONS_H
#define EXT_RMANPKG_22_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_MATFILT_CONVERSIONS_H

#include "wabi/base/gf/vec3f.h"
#include "wabi/base/tf/debug.h"
#include "wabi/base/tf/staticTokens.h"
#include "wabi/imaging/hd/sceneDelegate.h"
#include "wabi/imaging/hf/diagnostic.h"
#include "wabi/imaging/plugin/hdPrman/context.h"
#include "wabi/imaging/plugin/hdPrman/debugCodes.h"
#include "wabi/imaging/plugin/hdPrman/light.h"
#include "wabi/imaging/plugin/hdPrman/renderParam.h"
#include "wabi/imaging/plugin/hdPrman/rixStrings.h"
#include "wabi/usd/sdf/types.h"

#include "Riley.h"
#include "RixShadingUtils.h"

WABI_NAMESPACE_BEGIN

class HdPrman_Context;
class HdSceneDelegate;
class SdfPath;
class TfToken;

bool HdPrmanLightFilterPopulateNodesFromLightParams(std::vector<riley::ShadingNode> *filterNodes,
                                                    SdfPath &filterPath,
                                                    HdSceneDelegate *sceneDelegate);

void HdPrmanLightFilterGenerateCoordSysAndLinks(riley::ShadingNode *filter,
                                                SdfPath &filterPath,
                                                std::vector<riley::CoordinateSystemId> *coordsysIds,
                                                std::vector<TfToken> *filterLinks,
                                                HdSceneDelegate *sceneDelegate,
                                                HdPrman_Context *context,
                                                riley::Riley *riley,
                                                const riley::ShadingNode &lightNode);

WABI_NAMESPACE_END

#endif
