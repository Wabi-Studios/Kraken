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

/**
 * @file
 * Universe.
 * Set the Stage.
 */

#include "UNI_api.h"
#include "UNI_area.h"
#include "UNI_context.h"
#include "UNI_scene.h"
#include "UNI_screen.h"
#include "UNI_userpref.h"
#include "UNI_window.h"
#include "UNI_workspace.h"

#include "CKE_context.h"
#include "CKE_main.h"

#include "CLI_icons.h"

#include <wabi/wabi.h>

#include <wabi/base/vt/value.h>

#include <wabi/usd/usdUI/area.h>
#include <wabi/usd/usdUI/screen.h>
#include <wabi/usd/usdUI/tokens.h>
#include <wabi/usd/usdUI/window.h>
#include <wabi/usd/usdUI/workspace.h>

#include <wabi/usd/usdGeom/cube.h>
#include <wabi/usd/usdGeom/gprim.h>

#include <wabi/usd/usd/collectionAPI.h>
#include <wabi/usd/usd/stage.h>

WABI_NAMESPACE_BEGIN

void UNI_create_stage(cContext *C)
{
  Main *main = CTX_data_main(C);

  main->stage_id = TfStringCatPaths(main->temp_dir, "startup.usda");

  CTX_data_scene_set(C, new Scene(main->stage_id));
}

void UNI_destroy(cContext *C)
{
  Stage stage = CTX_data_stage(C);
  stage->~UsdStage();
}

void UNI_open_stage(cContext *C)
{
  Main *main = CTX_data_main(C);
  Stage stage = CTX_data_stage(C);

  stage->Open(main->stage_id);
}

void UNI_save_stage(cContext *C)
{
  Stage stage = CTX_data_stage(C);
  stage->GetRootLayer()->Save();
}

void UNI_set_defaults(cContext *C)
{
  /* ----- */

  Stage stage = CTX_data_stage(C);

  wmWindowManager *wm = new wmWindowManager();
  CTX_wm_manager_set(C, wm);

  /* ----- */

  /** Default Window. */
  wmWindow *win = new wmWindow(C);
  UniStageSetToken(win, title, "Covah");
  UniStageSetAsset(win, icon, CLI_icon(ICON_COVAH));
  UniStageSetToken(win, state, UsdUITokens->maximized);
  UniStageSetToken(win, cursor, UsdUITokens->default_);
  UniStageSetToken(win, alignment, UsdUITokens->alignAbsolute);
  UniStageSetVec2f(win, pos, (0.0, 0.0));
  UniStageSetVec2f(win, size, (1920, 1080));
  UniStageSetToken(win, type, UsdUITokens->normal);
  UniStageSetTarget(win, workspace_rel, win->prims.workspace->path);
  UniStageSetFlt(win, dpi, 1.0);
  UniStageSetFlt(win, dpifac, 1.0);
  UniStageSetFlt(win, widgetunit, 20.0);
  UniStageSetFlt(win, scale, 1.0);
  UniStageSetFlt(win, linewidth, 1.0);
  UniStageSetFlt(win, pixelsz, 1.0);
  wm->windows.insert(std::make_pair(win->path, win));
  CTX_wm_window_set(C, win);

  /* ----- */

  /** Default User Preferences. */
  UserDef *uprefs = new UserDef(C);
  UniStageSetBool(uprefs, showsave, true);
  CTX_data_prefs_set(C, uprefs);

  /* ----- */

  /** Default Viewport. */
  ScrArea *v3d = new ScrArea(C, win->prims.screen, SdfPath("View3D"));
  UniStageSetToken(v3d, name, "View3D");
  UniStageSetToken(v3d, spacetype, UsdUITokens->spaceView3D);
  UniStageSetAsset(v3d, icon, CLI_icon(ICON_HYDRA));
  UniStageSetVec2f(v3d, pos, (0, 0));
  UniStageSetVec2f(v3d, size, (1800, 1080));

  /* ----- */

  /** Default Outliner. */
  ScrArea *outliner = new ScrArea(C, win->prims.screen, SdfPath("Outliner"));
  UniStageSetToken(outliner, name, "Outliner");
  UniStageSetToken(outliner, spacetype, UsdUITokens->spaceOutliner);
  UniStageSetAsset(outliner, icon, CLI_icon(ICON_LUXO));
  UniStageSetVec2f(outliner, pos, (1800, 0));
  UniStageSetVec2f(outliner, size, (120, 1080));

  /* ----- */

  /** Add UI Areas to Screen's Collection of Areas. */
  win->prims.screen->align.Set(UsdUITokens->verticalSplit);
  UniStageSetToken(win->prims.screen, align, UsdUITokens->verticalSplit);
  UniStageSetTarget(win->prims.screen, areas_rel, v3d->path);
  UniStageSetTarget(win->prims.screen, areas_rel, outliner->path);
  CTX_wm_screen_set(C, win->prims.screen);


  /** Add this screen to our default 'Layout' WorkSpace. */
  UniStageSetToken(win->prims.workspace, name, "Layout");
  UniStageSetTarget(win->prims.workspace, screen_rel, win->prims.screen->path);

  /* ----- */
}

void UNI_author_default_scene(cContext *C)
{
  Stage stage = CTX_data_stage(C);

  UsdGeomCube cube = UsdGeomCube::Define(stage, SdfPath("/Cube"));

  UsdGeomXformOp location = cube.AddTranslateOp();
  location.Set(VtValue(GfVec3d(0.0f, 0.0f, 0.0f)));
  UsdGeomXformOp rotation = cube.AddRotateXYZOp();
  rotation.Set(VtValue(GfVec3f(0.0f, 0.0f, 0.0f)));
  UsdGeomXformOp scale = cube.AddScaleOp();
  scale.Set(VtValue(GfVec3f(1.0f, 1.0f, 1.0f)));
}

WABI_NAMESPACE_END