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
#include "wabi/usd/usdUI/tokens.h"

WABI_NAMESPACE_BEGIN

UsdUITokensType::UsdUITokensType()
  : alignAbsolute("alignAbsolute", TfToken::Immortal),
    alignCenter("alignCenter", TfToken::Immortal),
    alignParent("alignParent", TfToken::Immortal),
    areas("areas", TfToken::Immortal),
    bottom("bottom", TfToken::Immortal),
    closed("closed", TfToken::Immortal),
    copy("copy", TfToken::Immortal),
    cross("cross", TfToken::Immortal),
    default_("default", TfToken::Immortal),
    detached("detached", TfToken::Immortal),
    dialog("dialog", TfToken::Immortal),
    dot("dot", TfToken::Immortal),
    edit("edit", TfToken::Immortal),
    embedded("embedded", TfToken::Immortal),
    eraser("eraser", TfToken::Immortal),
    eyedropper("eyedropper", TfToken::Immortal),
    fullscreen("fullscreen", TfToken::Immortal),
    hand("hand", TfToken::Immortal),
    horizontalSplit("horizontalSplit", TfToken::Immortal),
    hSplit("hSplit", TfToken::Immortal),
    knife("knife", TfToken::Immortal),
    left("left", TfToken::Immortal),
    maximized("maximized", TfToken::Immortal),
    minimized("minimized", TfToken::Immortal),
    none("none", TfToken::Immortal),
    normal("normal", TfToken::Immortal),
    open("open", TfToken::Immortal),
    paint("paint", TfToken::Immortal),
    paintBrush("paintBrush", TfToken::Immortal),
    right("right", TfToken::Immortal),
    spaceClip("SpaceClip", TfToken::Immortal),
    spaceConsole("SpaceConsole", TfToken::Immortal),
    spaceEmpty("SpaceEmpty", TfToken::Immortal),
    spaceFile("SpaceFile", TfToken::Immortal),
    spaceGraph("SpaceGraph", TfToken::Immortal),
    spaceImage("SpaceImage", TfToken::Immortal),
    spaceInfo("SpaceInfo", TfToken::Immortal),
    spaceNode("SpaceNode", TfToken::Immortal),
    spaceOutliner("SpaceOutliner", TfToken::Immortal),
    spacePref("SpacePref", TfToken::Immortal),
    spaceProperties("SpaceProperties", TfToken::Immortal),
    spaceSequence("SpaceSequence", TfToken::Immortal),
    spaceSpreadsheet("SpaceSpreadsheet", TfToken::Immortal),
    spaceStatusbar("SpaceStatusbar", TfToken::Immortal),
    spaceText("SpaceText", TfToken::Immortal),
    spaceTopbar("SpaceTopbar", TfToken::Immortal),
    spaceView3D("SpaceView3D", TfToken::Immortal),
    stop("stop", TfToken::Immortal),
    swapArea("swapArea", TfToken::Immortal),
    textEdit("textEdit", TfToken::Immortal),
    top("top", TfToken::Immortal),
    uiAreaIcon("ui:area:icon", TfToken::Immortal),
    uiAreaName("ui:area:name", TfToken::Immortal),
    uiAreaPos("ui:area:pos", TfToken::Immortal),
    uiAreaSize("ui:area:size", TfToken::Immortal),
    uiAreaSpacetype("ui:area:spacetype", TfToken::Immortal),
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
    uiScreenCollectionAreasIncludeRoot("ui:screen:collection:areas:includeRoot",
                                       TfToken::Immortal),
    uiUserprefDpifac("ui:userpref:dpifac", TfToken::Immortal),
    uiUserprefShowSavePrompt("ui:userpref:showSavePrompt", TfToken::Immortal),
    uiWindowAlignment("ui:window:alignment", TfToken::Immortal),
    uiWindowCursor("ui:window:cursor", TfToken::Immortal),
    uiWindowDpi("ui:window:dpi", TfToken::Immortal),
    uiWindowIcon("ui:window:icon", TfToken::Immortal),
    uiWindowLinewidth("ui:window:linewidth", TfToken::Immortal),
    uiWindowPixelsz("ui:window:pixelsz", TfToken::Immortal),
    uiWindowPos("ui:window:pos", TfToken::Immortal),
    uiWindowScale("ui:window:scale", TfToken::Immortal),
    uiWindowSize("ui:window:size", TfToken::Immortal),
    uiWindowState("ui:window:state", TfToken::Immortal),
    uiWindowTitle("ui:window:title", TfToken::Immortal),
    uiWindowType("ui:window:type", TfToken::Immortal),
    uiWindowWidgetunit("ui:window:widgetunit", TfToken::Immortal),
    uiWindowWorkspace("ui:window:workspace", TfToken::Immortal),
    uiWorkspaceName("ui:workspace:name", TfToken::Immortal),
    uiWorkspaceScreen("ui:workspace:screen", TfToken::Immortal),
    verticalSplit("verticalSplit", TfToken::Immortal),
    vloop("vloop", TfToken::Immortal),
    vSplit("vSplit", TfToken::Immortal),
    wait("wait", TfToken::Immortal),
    xMove("xMove", TfToken::Immortal),
    yMove("yMove", TfToken::Immortal),
    zoomIn("zoomIn", TfToken::Immortal),
    zoomOut("zoomOut", TfToken::Immortal),
    allTokens({alignAbsolute,
               alignCenter,
               alignParent,
               areas,
               bottom,
               closed,
               copy,
               cross,
               default_,
               detached,
               dialog,
               dot,
               edit,
               embedded,
               eraser,
               eyedropper,
               fullscreen,
               hand,
               horizontalSplit,
               hSplit,
               knife,
               left,
               maximized,
               minimized,
               none,
               normal,
               open,
               paint,
               paintBrush,
               right,
               spaceClip,
               spaceConsole,
               spaceEmpty,
               spaceFile,
               spaceGraph,
               spaceImage,
               spaceInfo,
               spaceNode,
               spaceOutliner,
               spacePref,
               spaceProperties,
               spaceSequence,
               spaceSpreadsheet,
               spaceStatusbar,
               spaceText,
               spaceTopbar,
               spaceView3D,
               stop,
               swapArea,
               textEdit,
               top,
               uiAreaIcon,
               uiAreaName,
               uiAreaPos,
               uiAreaSize,
               uiAreaSpacetype,
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
               uiUserprefDpifac,
               uiUserprefShowSavePrompt,
               uiWindowAlignment,
               uiWindowCursor,
               uiWindowDpi,
               uiWindowIcon,
               uiWindowLinewidth,
               uiWindowPixelsz,
               uiWindowPos,
               uiWindowScale,
               uiWindowSize,
               uiWindowState,
               uiWindowTitle,
               uiWindowType,
               uiWindowWidgetunit,
               uiWindowWorkspace,
               uiWorkspaceName,
               uiWorkspaceScreen,
               verticalSplit,
               vloop,
               vSplit,
               wait,
               xMove,
               yMove,
               zoomIn,
               zoomOut})
{}

TfStaticData<UsdUITokensType> UsdUITokens;

WABI_NAMESPACE_END
