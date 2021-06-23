/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright 2021, Wabi.
 */

#pragma once

/**
 * @file
 * Universe.
 * Set the Stage.
 */

#include "UNI_area.h"
#include "UNI_context.h"
#include "UNI_region.h"
#include "UNI_screen.h"
#include "UNI_wm_types.h"
#include "UNI_workspace.h"

#include "WM_operators.h"

#include "CKE_context.h"
#include "CKE_robinhood.h"

#include <wabi/usd/sdf/path.h>
#include <wabi/usd/usd/collectionAPI.h>
#include <wabi/usd/usdUI/window.h>

#include <deque>

WABI_NAMESPACE_BEGIN

typedef std::deque<wmEvent *> wmEventQueue;

struct ScrAreaMap
{
  /** ScrVert. */
  std::vector<struct ScrVert *> verts;
  /** ScrEdge. */
  std::vector<struct ScrEdge *> edges;
  /** ScrArea. */
  std::vector<struct ScrArea *> areas;

  ScrAreaMap()
    : verts(EMPTY),
      edges(EMPTY),
      areas(EMPTY)
  {}
};

struct wmWindow : public UsdUIWindow, public UniverseObject
{
  SdfPath path;
  wmWindow *parent;

  UsdAttribute title;
  UsdAttribute icon;
  UsdAttribute state;
  UsdAttribute dpi;
  UsdAttribute widgetunit;
  UsdAttribute scale;
  UsdAttribute linewidth;
  UsdAttribute pixelsz;
  UsdAttribute cursor;
  UsdAttribute pos;
  UsdAttribute alignment;
  UsdAttribute size;
  UsdAttribute type;

  UsdRelationship workspace_rel;

  /** Active scene for this window. */
  TfToken scene;

  /** Anchor system backend pointer. */
  void *anchorwin;

  /** Set to one for an active window. */
  bool active;

  bool addmousemove;

  /** Storage for event system. */
  wmEvent *eventstate;
  wmEventQueue event_queue;

  /** Runtime Window State. */
  char windowstate;
  int winid;

  WorkSpaceInstanceHook *workspace_hook;

  ScrAreaMap global_areas;

  inline wmWindow(cContext *C, const SdfPath &stagepath);

  inline wmWindow(cContext *C, wmWindow *prim, const SdfPath &stagepath);
};


wmWindow::wmWindow(cContext *C, const SdfPath &stagepath)
  : UsdUIWindow(COVAH_UNIVERSE_CREATE(C)),
    path(stagepath),
    parent(NULL),
    title(CreateTitleAttr()),
    icon(CreateIconAttr()),
    state(CreateStateAttr()),
    dpi(CreateDpiAttr()),
    widgetunit(CreateWidgetunitAttr()),
    scale(CreateScaleAttr()),
    linewidth(CreateLinewidthAttr()),
    pixelsz(CreatePixelszAttr()),
    cursor(CreateCursorAttr()),
    pos(CreatePosAttr()),
    alignment(CreateAlignmentAttr()),
    size(CreateSizeAttr()),
    type(CreateTypeAttr()),
    workspace_rel(CreateUiWindowWorkspaceRel()),
    anchorwin(nullptr),
    active(true),
    addmousemove(false),
    eventstate(new wmEvent()),
    windowstate(0),
    winid(0),
    workspace_hook(nullptr)
{}

wmWindow::wmWindow(cContext *C, wmWindow *prim, const SdfPath &stagepath)
  : UsdUIWindow(COVAH_UNIVERSE_CREATE_CHILD(C)),
    path(UsdUIWindow::GetPath()),
    parent(prim),
    title(CreateTitleAttr()),
    icon(CreateIconAttr()),
    state(CreateStateAttr()),
    dpi(CreateDpiAttr()),
    widgetunit(CreateWidgetunitAttr()),
    scale(CreateScaleAttr()),
    linewidth(CreateLinewidthAttr()),
    pixelsz(CreatePixelszAttr()),
    cursor(CreateCursorAttr()),
    pos(CreatePosAttr()),
    alignment(CreateAlignmentAttr()),
    size(CreateSizeAttr()),
    type(CreateTypeAttr()),
    workspace_rel(CreateUiWindowWorkspaceRel()),
    anchorwin(nullptr),
    active(true),
    addmousemove(false),
    eventstate(new wmEvent()),
    windowstate(0),
    winid(0),
    workspace_hook(nullptr)
{}

struct wmNotifier
{
  wmWindow *window;

  unsigned int category, data, subtype, action;

  void *reference;

  inline wmNotifier();
};

wmNotifier::wmNotifier()
  : window(nullptr),
    category(0),
    data(0),
    subtype(0),
    action(0),
    reference(nullptr)
{}

struct wmSpaceTypeListenerParams
{
  struct wmWindow *window;
  struct ScrArea *area;
  struct wmNotifier *notifier;
  const struct Scene *scene;

  wmSpaceTypeListenerParams()
    : window(POINTER_ZERO),
      area(POINTER_ZERO),
      notifier(POINTER_ZERO),
      scene(POINTER_ZERO)
  {}
};

typedef std::deque<wmNotifier *> wmNotifierQueue;

struct wmWindowManager : public UniverseObject
{
  SdfPath path;

  /** All windows this manager controls. */
  TfHashMap<SdfPath, wmWindow *, SdfPath::Hash> windows;

  wmWindow *windrawable, *winactive;

  wmNotifierQueue notifier_queue;

  int op_undo_depth;

  /** Active dragged items. */
  std::vector<void *> drags;

  inline wmWindowManager();
};

wmWindowManager::wmWindowManager()
  : path(COVAH_PATH_DEFAULTS::COVAH_WM),
    windows(),
    windrawable(nullptr),
    winactive(nullptr),
    notifier_queue(),
    op_undo_depth(0)
{}

WABI_NAMESPACE_END