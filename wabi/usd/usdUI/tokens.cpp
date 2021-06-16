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

/* clang-format off */

#include "wabi/usd/usdUI/tokens.h"

WABI_NAMESPACE_BEGIN

UsdUITokensType::UsdUITokensType()
  : areas("areas", TfToken::Immortal),
    bottom("bottom", TfToken::Immortal),
    closed("closed", TfToken::Immortal),
    detached("detached", TfToken::Immortal),
    dialog("dialog", TfToken::Immortal),
    horizontalSplit("horizontalSplit", TfToken::Immortal),
    left("left", TfToken::Immortal),
    minimized("minimized", TfToken::Immortal),
    none("none", TfToken::Immortal),
    normal("normal", TfToken::Immortal),
    open("open", TfToken::Immortal),
    right("right", TfToken::Immortal),
    top("top", TfToken::Immortal),
    uiAreaIcon("ui:area:icon", TfToken::Immortal),
    uiAreaName("ui:area:name", TfToken::Immortal),
    uiAreaPos("ui:area:pos", TfToken::Immortal),
    uiAreaSize("ui:area:size", TfToken::Immortal),
    uiDescription("ui:description", TfToken::Immortal),
    uiDisplayGroup("ui:displayGroup", TfToken::Immortal),
    uiDisplayName("ui:displayName", TfToken::Immortal),
    uiNodegraphNodeDisplayColor("ui:nodegraph:node:displayColor", TfToken::Immortal),
    uiNodegraphNodeExpansionState("ui:nodegraph:node:expansionState", TfToken::Immortal),
    uiNodegraphNodeIcon("ui:nodegraph:node:icon", TfToken::Immortal),
    uiNodegraphNodePos("ui:nodegraph:node:pos", TfToken::Immortal),
    uiNodegraphNodeSize("ui:nodegraph:node:size", TfToken::Immortal),
    uiNodegraphNodeStackingOrder("ui:nodegraph:node:stackingOrder", TfToken::Immortal),
    uiScreenAlignment("ui:screen:alignment", TfToken::Immortal),
    uiScreenAreas("ui:screen:areas", TfToken::Immortal),
    uiScreenCollectionAreasIncludeRoot("ui:screen:collection:areas:includeRoot", TfToken::Immortal),
    uiWindowIcon("ui:window:icon", TfToken::Immortal),
    uiWindowPos("ui:window:pos", TfToken::Immortal),
    uiWindowSize("ui:window:size", TfToken::Immortal),
    uiWindowTitle("ui:window:title", TfToken::Immortal),
    uiWindowType("ui:window:type", TfToken::Immortal),
    uiWindowWorkspace("ui:window:workspace", TfToken::Immortal),
    uiWorkspaceName("ui:workspace:name", TfToken::Immortal),
    uiWorkspaceScreen("ui:workspace:screen", TfToken::Immortal),
    verticalSplit("verticalSplit", TfToken::Immortal),
    allTokens({
      areas,
      bottom,
      closed,
      detached,
      dialog,
      horizontalSplit,
      left,
      minimized,
      none,
      normal,
      open,
      right,
      top,
      uiAreaIcon,
      uiAreaName,
      uiAreaPos,
      uiAreaSize,
      uiDescription,
      uiDisplayGroup,
      uiDisplayName,
      uiNodegraphNodeDisplayColor,
      uiNodegraphNodeExpansionState,
      uiNodegraphNodeIcon,
      uiNodegraphNodePos,
      uiNodegraphNodeSize,
      uiNodegraphNodeStackingOrder,
      uiScreenAlignment,
      uiScreenAreas,
      uiScreenCollectionAreasIncludeRoot,
      uiWindowIcon,
      uiWindowPos,
      uiWindowSize,
      uiWindowTitle,
      uiWindowType,
      uiWindowWorkspace,
      uiWorkspaceName,
      uiWorkspaceScreen,
      verticalSplit})
{}

TfStaticData<UsdUITokensType> UsdUITokens;

WABI_NAMESPACE_END
