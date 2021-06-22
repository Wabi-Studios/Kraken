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
 * Window Manager.
 * Making GUI Fly.
 */

#include "WM_window.h"
#include "WM_cursors_api.h"
#include "WM_debug_codes.h"
#include "WM_event_system.h"
#include "WM_inline_tools.h"
#include "WM_operators.h"
#include "WM_tokens.h"

#include "UNI_area.h"
#include "UNI_context.h"
#include "UNI_factory.h"
#include "UNI_object.h"
#include "UNI_operator.h"
#include "UNI_userpref.h"
#include "UNI_window.h"
#include "UNI_workspace.h"

#include "ANCHOR_api.h"
#include "ANCHOR_event_consumer.h"
#include "ANCHOR_system.h"

#include "CKE_context.h"
#include "CKE_main.h"

#include "CLI_icons.h"
#include "CLI_math_inline.h"
#include "CLI_string_utils.h"
#include "CLI_time.h"

#include <wabi/base/gf/vec2f.h>

WABI_NAMESPACE_BEGIN


/* handle to anchor system. */
static ANCHOR_SystemHandle anchor_system = NULL;


enum eModifierKeyType
{
  SHIFT = 's',
  CONTROL = 'c',
  ALT = 'a',
  OS = 'C',
};


static int query_qual(eModifierKeyType qual)
{
  eAnchorModifierKeyMask left, right;
  switch (qual)
  {
    case SHIFT:
      left = ANCHOR_ModifierKeyLeftShift;
      right = ANCHOR_ModifierKeyRightShift;
      break;
    case CONTROL:
      left = ANCHOR_ModifierKeyLeftControl;
      right = ANCHOR_ModifierKeyRightControl;
      break;
    case OS:
      left = right = ANCHOR_ModifierKeyOS;
      break;
    case ALT:
    default:
      left = ANCHOR_ModifierKeyLeftAlt;
      right = ANCHOR_ModifierKeyRightAlt;
      break;
  }

  int val = 0;
  ANCHOR::GetModifierKeyState(anchor_system, left, &val);
  if (!val)
  {
    ANCHOR::GetModifierKeyState(anchor_system, right, &val);
  }

  return val;
}


void wm_cursor_position_get(wmWindow *win, int *r_x, int *r_y)
{
  ANCHOR::GetCursorPosition(anchor_system, r_x, r_y);
  WM_cursor_position_from_anchor(win, r_x, r_y);
}


static void wm_window_set_drawable(wmWindowManager *wm, wmWindow *win, bool activate)
{
  // CLI_assert(ELEM(wm->windrawable, NULL, win));

  wm->windrawable = win;
  // if (activate) {
  //   ANCHOR::ActivateWindowDrawingContext(win->anchorwin);
  // }
  // GPU_context_active_set(win->gpuctx);
}


static void wm_window_set_dpi(wmWindow *win)
{
  float win_scale = FormFactory(win->scale);
  float win_linewidth = FormFactory(win->linewidth);

  float auto_dpi = ANCHOR::GetDPIHint((ANCHOR_SystemWindowHandle)win->anchorwin);

  auto_dpi = max_ff(auto_dpi, 96.0f);
  auto_dpi *= ANCHOR::GetNativePixelSize((ANCHOR_SystemWindowHandle)win->anchorwin);
  int dpi = auto_dpi * win_scale * (72.0 / 96.0f);

  int pixelsize = max_ii(1, (int)(dpi / 64));
  pixelsize = max_ii(1, pixelsize + win_linewidth);

  float dpiadj = dpi / pixelsize;
  float dpifac = (pixelsize * (float)(dpiadj)) / 72.0f;
  float wunit = (pixelsize * (dpiadj / pixelsize) * 20 + 36) / 72;

  /* ----- */

  /**
   * Set prefs on
   * Pixar Stage. */
  FormFactory(win->pixelsz, float(pixelsize));
  FormFactory(win->dpi, float(dpiadj));
  FormFactory(win->dpifac, float(dpifac));
  FormFactory(win->widgetunit, float(wunit += 2 * ((int)pixelsize - (int)dpifac)));

  /* ----- */

  /* update font drawing */
  ANCHOR::GetIO().FontGlobalScale = pixelsize * dpiadj;
}


void wm_window_clear_drawable(wmWindowManager *wm)
{
  if (wm->windrawable)
  {
    wm->windrawable = NULL;
  }
}

void wm_window_make_drawable(wmWindowManager *wm, wmWindow *win)
{
  // if (win != wm->windrawable && win->anchorwin)
  // {
  //   wm_window_clear_drawable(wm);

  //   wm_window_set_drawable(wm, win, true);

  //   /* this can change per window */
  //   wm_window_set_dpi(win);
  // }
}


static void wm_window_update_eventstate(wmWindow *win)
{
  /* Update mouse position when a window is activated. */
  wm_cursor_position_get(win, &GET_X(win->eventstate->mouse_pos), &GET_Y(win->eventstate->mouse_pos));
}


/**
 * This is called by anchor, and this is where
 * we handle events for windows or send them to
 * the event system. */
static int anchor_event_proc(ANCHOR_EventHandle evt, ANCHOR_UserPtr C_void_ptr)
{
  cContext *C = (cContext *)C_void_ptr;
  wmWindowManager *wm = CTX_wm_manager(C);
  eAnchorEventType type = ANCHOR::GetEventType(evt);

  if (type == ANCHOR_EventTypeQuitRequest)
  {
    /* Find an active window to display quit dialog in. */
    ANCHOR_SystemWindowHandle anchorwin = ANCHOR::GetEventWindow(evt);

    wmWindow *win = nullptr;
    if (anchorwin && ANCHOR::ValidWindow(anchor_system, anchorwin))
    {
      win = wm->windows.begin()->second;
      win->anchorwin = anchorwin;
    }
    else
    {
      win = wm->windows.begin()->second;
    }

    /* Display quit dialog or quit immediately. */
    if (win)
    {
      wm_quit_with_optional_confirmation_prompt(C, win);
    }
    else
    {
      wm_exit_schedule_delayed(C);
    }
  }
  else
  {
    ANCHOR_SystemWindowHandle anchorwin = ANCHOR::GetEventWindow(evt);
    ANCHOR_EventDataPtr data = ANCHOR::GetEventData(evt);

    if (!anchorwin)
    {
      puts("<!> event has no window");
      return 1;
    }

    if (!ANCHOR::ValidWindow(anchor_system, anchorwin))
    {
      puts("<!> event has invalid window");
      return 1;
    }

    wmWindow *win = wm->windows.begin()->second;
    win->anchorwin = anchorwin;

    switch (type)
    {
      case ANCHOR_EventTypeWindowDeactivate:
        WM_event_add_anchorevent(wm, win, type, data);
        win->active = false;

        win->eventstate->alt = 0;
        win->eventstate->ctrl = 0;
        win->eventstate->shift = 0;
        win->eventstate->oskey = 0;
        win->eventstate->keymodifier = 0;
        break;
      case ANCHOR_EventTypeWindowActivate: {
        ANCHOR_EventKeyData kdata;
        const int keymodifier = ((query_qual(SHIFT) ? KM_SHIFT : 0) |
                                 (query_qual(CONTROL) ? KM_CTRL : 0) |
                                 (query_qual(ALT) ? KM_ALT : 0) | (query_qual(OS) ? KM_OSKEY : 0));
        wm->winactive = win;
        win->active = true;

        kdata.ascii = '\0';
        kdata.utf8_buf[0] = '\0';

        if (win->eventstate->shift)
        {
          if ((keymodifier & KM_SHIFT) == 0)
          {
            kdata.key = ANCHOR_KeyLeftShift;
            WM_event_add_anchorevent(wm, win, ANCHOR_EventTypeKeyUp, &kdata);
          }
        }

        if (win->eventstate->ctrl)
        {
          if ((keymodifier & KM_CTRL) == 0)
          {
            kdata.key = ANCHOR_KeyLeftControl;
            WM_event_add_anchorevent(wm, win, ANCHOR_EventTypeKeyUp, &kdata);
          }
        }

        if (win->eventstate->alt)
        {
          if ((keymodifier & KM_ALT) == 0)
          {
            kdata.key = ANCHOR_KeyLeftAlt;
            WM_event_add_anchorevent(wm, win, ANCHOR_EventTypeKeyUp, &kdata);
          }
        }

        if (win->eventstate->oskey)
        {
          if ((keymodifier & KM_OSKEY) == 0)
          {
            kdata.key = ANCHOR_KeyOS;
            WM_event_add_anchorevent(wm, win, ANCHOR_EventTypeKeyUp, &kdata);
          }
        }

        /* keymodifier zero, it hangs on hotkeys that open windows otherwise */
        win->eventstate->keymodifier = 0;

        /* entering window, update mouse pos. but no event */
        wm_window_update_eventstate(win);

        win->addmousemove = true; /* enables highlighted buttons */

        wm_window_make_drawable(wm, win);

        wmEvent event;
        WM_event_init_from_window(win, &event);
        event.type = MOUSEMOVE;
        event.prev_mouse_pos = event.mouse_pos;
        event.is_repeat = false;

        wm_event_add(win, &event);

        break;
      }
      case ANCHOR_EventTypeWindowClose: {
        wm_window_close(C, wm, win);
        break;
      }
      case ANCHOR_EventTypeWindowUpdate: {
        wm_window_make_drawable(wm, win);
        WM_event_add_notifier(C, NC_WINDOW, NULL);

        break;
      }
      default: {
        // WM_event_add_anchorevent(wm, win, type, data);
        break;
      }
    }
  }

  return 1;
}

static void wm_window_anchorwindow_add(wmWindowManager *wm, wmWindow *win, bool is_dialog)
{

  /* ----- */

  /**
   * This comes direct
   * from Pixar Stage. */
  std::string();
  TfToken win_title = FormFactory(win->title);
  SdfAssetPath win_icon = FormFactory(win->icon);
  GfVec2f win_pos = FormFactory(win->pos);
  GfVec2f win_size = FormFactory(win->size);

  /* ----- */

  ANCHOR_SystemWindowHandle anchorwin = ANCHOR::CreateWindow(anchor_system,
                                                             (win->parent) ? (ANCHOR_SystemWindowHandle)win->parent->anchorwin : NULL,
                                                             CHARALL(win_title),
                                                             CHARALL(win_icon.GetAssetPath()),
                                                             GET_X(win_pos),
                                                             GET_Y(win_pos),
                                                             GET_X(win_size),
                                                             GET_Y(win_size),
                                                             ANCHOR_WindowStateFullScreen,
                                                             is_dialog,
                                                             ANCHOR_DrawingContextTypeVulkan,
                                                             0);
  if (anchorwin)
  {
    win->anchorwin = anchorwin;
  }
}


static void wm_window_anchorwindow_ensure(wmWindowManager *wm, wmWindow *win, bool is_dialog)
{
  if (!win->anchorwin)
  {
    /* ----- */

    /**
     * This comes direct
     * from Pixar Stage. */

    TfToken win_title = FormFactory(win->title);
    SdfAssetPath win_icon = FormFactory(win->icon);
    TfToken win_cursor = FormFactory(win->cursor);
    GfVec2f win_pos = FormFactory(win->pos);
    GfVec2f win_size = FormFactory(win->size);

    /* ----- */

    if ((GET_X(win_size) == 0))
    {
      FormFactory(win->pos, GfVec2f(0.0, 0.0));
      FormFactory(win->size, GfVec2f(1920, 1080));
      FormFactory(win->state, UsdUITokens->maximized);
      FormFactory(win->alignment, UsdUITokens->alignAbsolute);

      if (win_cursor.IsEmpty())
      {
        FormFactory(win->cursor, UsdUITokens->default_);
      }

      if (win_title.IsEmpty())
      {
        FormFactory(win->title, TfToken("Covah"));
      }

      if (win_icon.GetAssetPath().empty())
      {
        FormFactory(win->icon, SdfAssetPath(CLI_icon(ICON_HYDRA)));
      }
    }

    wm_window_anchorwindow_add(wm, win, is_dialog);
    wm_window_set_dpi(win);
  }
}

static void wm_get_screensize(int *r_width, int *r_height)
{
  unsigned int uiwidth;
  unsigned int uiheight;

  ANCHOR::GetMainDisplayDimensions(anchor_system, &uiwidth, &uiheight);
  *r_width = uiwidth;
  *r_height = uiheight;
}


void WM_window_anchorwindows_ensure(wmWindowManager *wm)
{
  UNIVERSE_FOR_ALL(win, wm->windows)
  {
    wm_window_anchorwindow_ensure(wm, VALUE(win), false);
  }
}


/* keeps size within monitor bounds */
static void wm_window_check_size(GfVec4i *rect)
{
  int width, height;
  wm_get_screensize(&width, &height);

  int xmin = rect->GetArray()[0];
  int ymin = rect->GetArray()[1];
  int xmax = rect->GetArray()[2];
  int ymax = rect->GetArray()[3];

  int sizex = (xmax - xmin);
  int sizey = (ymax - ymin);

  if (sizex > width)
  {
    int centx = (xmin + xmax) / 2;
    xmin = centx - (width / 2);
    xmax = xmin + width;
  }

  if (sizey > height)
  {
    int centy = (ymin + ymax) / 2;
    ymin = centy - (height / 2);
    ymax = ymin + height;
  }

  rect->Set(xmin, ymin, xmax, ymax);
}


/**
 * @param space_type: SPACE_VIEW3D, SPACE_INFO, ... (eSpace_Type)
 * @param dialog: whether this should be made as a dialog-style window
 * @param temp: whether this is considered a short-lived window
 * @param alignment: how this window is positioned relative to its parent
 * @return the window or NULL in case of failure. */
wmWindow *WM_window_open(cContext *C,
                         const char *title,
                         const char *icon,
                         int x,
                         int y,
                         int sizex,
                         int sizey,
                         TfToken space_type,
                         TfToken alignment,
                         bool dialog,
                         bool temp)
{
  Main *cmain = CTX_data_main(C);
  wmWindowManager *wm = CTX_wm_manager(C);
  wmWindow *win_prev = CTX_wm_window(C);
  Scene *scene = CTX_data_scene(C);
  GfVec4i rect;

  GfVec2f pos = FormFactory(win_prev->pos);
  GfVec2f size = FormFactory(win_prev->size);

  const float native_pixel_size = ANCHOR::GetNativePixelSize((ANCHOR_SystemWindowHandle)win_prev->anchorwin);
  /* convert to native OS window coordinates */
  rect[0] = pos[0] + (x / native_pixel_size);
  rect[1] = pos[1] + (y / native_pixel_size);
  sizex /= native_pixel_size;
  sizey /= native_pixel_size;

  if (alignment == UsdUITokens->alignCenter)
  {
    /* Window centered around x,y location. */
    rect[0] -= sizex / 2;
    rect[1] -= sizey / 2;
  }
  else if (alignment == UsdUITokens->alignParent)
  {
    /* Centered within parent. X,Y as offsets from there. */
    rect[0] += (size[0] - sizex) / 2;
    rect[1] += (size[1] - sizey) / 2;
  }
  else
  {
    /* Positioned absolutely within parent bounds. */
  }

  rect[2] = rect[0] + sizex;
  rect[3] = rect[1] + sizey;

  /* changes rect to fit within desktop */
  wm_window_check_size(&rect);

  /* ----- */

  /**
   * Create Window. */
  wmWindow *win = new wmWindow(C, win_prev, SdfPath("Child"));
  wm->windows.insert(std::make_pair(win->path, win));

  /**
   * Dialogs may have a child window as parent.
   * Otherwise, a child must not be a parent too. */
  win->parent = (!dialog && win_prev && win_prev->parent) ? win_prev->parent : win_prev;

  /* ----- */

  win->pos.Set(GfVec2f(rect[0], rect[1]));
  win->size.Set(GfVec2f(rect[2] - rect[0], rect[3] - rect[1]));

  if (!win->prims.workspace->GetPrim().IsValid())
  {
    win->prims.workspace = win_prev->prims.workspace;
  }

  if (!win->prims.screen->areas_rel.HasAuthoredTargets())
  {
    /* add new screen layout */
  }

  CTX_wm_window_set(C, win);
  const bool new_window = (win->anchorwin == NULL);
  if (new_window)
  {
    wm_window_anchorwindow_ensure(wm, win, dialog);
  }
  WM_check(C);

  return win;
}


static void wm_close_file_dialog(cContext *C, wmGenericCallback *post_action)
{
  /**
   * TODO. */

  post_action->free_user_data(post_action->user_data);

  delete post_action;
}


void wm_exit_schedule_delayed(cContext *C)
{
  wmWindow *win = CTX_wm_window(C);

  /**
   * TODO. */

  exit(COVAH_SUCCESS);
}


static void wm_save_file_on_quit_dialog_callback(cContext *C, void *UNUSED(user_data))
{
  wm_exit_schedule_delayed(C);
}

static void wm_confirm_quit(cContext *C)
{
  /** Save User's Work. */
  TF_DEBUG(COVAH_DEBUG_OPERATORS).Msg("WARNING: Saving without a confirmation Dialog. Overwriting existing changes.");

  Stage stage = CTX_data_stage(C);
  if (stage->GetRootLayer()->Save())
  {
    wmGenericCallback *action = new wmGenericCallback;
    action->exec = (wmGenericCallbackFn)wm_save_file_on_quit_dialog_callback;
    wm_close_file_dialog(C, action);
  }
}


static void wm_window_raise(wmWindow *win)
{
  /* Restore window if minimized */
  if (ANCHOR::GetWindowState((ANCHOR_SystemWindowHandle)win->anchorwin) == ANCHOR_WindowStateMinimized)
  {
    ANCHOR::SetWindowState((ANCHOR_SystemWindowHandle)win->anchorwin, ANCHOR_WindowStateNormal);
  }
  ANCHOR::SetWindowOrder((ANCHOR_SystemWindowHandle)win->anchorwin, ANCHOR_kWindowOrderTop);
}


static void wm_window_desktop_pos_get(wmWindow *win,
                                      const GfVec2f screen_pos,
                                      GfVec2i *r_desk_pos)
{
  float win_pixelsz = FormFactory(win->pixelsz);
  GfVec2f win_pos = FormFactory(win->pos);

  /* To desktop space. */
  VEC2_SET(r_desk_pos,
           (GET_X(screen_pos) + (int)(win_pixelsz * GET_X(win_pos))),
           (GET_Y(screen_pos) + (int)(win_pixelsz * GET_Y(win_pos))));
}

static void wm_window_screen_pos_get(wmWindow *win,
                                     const GfVec2i desktop_pos,
                                     GfVec2i *r_scr_pos)
{
  float win_pixelsz = FormFactory(win->pixelsz);
  GfVec2f win_pos = FormFactory(win->pos);

  /* To window space. */
  VEC2_SET(r_scr_pos,
           (GET_X(desktop_pos) - (int)(win_pixelsz * GET_X(win_pos))),
           (GET_Y(desktop_pos) - (int)(win_pixelsz * GET_Y(win_pos))));
}


bool WM_window_find_under_cursor(wmWindowManager *wm,
                                 wmWindow *win_ignore,
                                 wmWindow *win,
                                 const GfVec2i mval,
                                 wmWindow **r_win,
                                 GfVec2i *r_mval)
{
  GfVec2i desk_pos;
  wm_window_desktop_pos_get(win, mval, &desk_pos);

  /* TODO: This should follow the order of the activated windows.
   * The current solution is imperfect but usable in most cases. */
  UNIVERSE_FOR_ALL(win_iter, wm->windows)
  {
    if (VALUE(win_iter) == win_ignore)
    {
      continue;
    }

    TfToken win_state = FormFactory(win->state);

    if (win_state == UsdUITokens->minimized)
    {
      continue;
    }

    GfVec2i scr_pos;
    wm_window_screen_pos_get(VALUE(win_iter), desk_pos, &scr_pos);

    GfVec2f win_pos = FormFactory(win->pos);

    if (GET_X(scr_pos) >= 0 && GET_Y(win_pos) >= 0 &&
        GET_X(scr_pos) <= WM_window_pixels_x(VALUE(win_iter)) &&
        GET_Y(scr_pos) <= WM_window_pixels_y(VALUE(win_iter)))
    {
      *r_win = VALUE(win_iter);

      VEC2_SET(r_mval, GET_X(scr_pos), GET_Y(scr_pos));
      return true;
    }
  }

  return false;
}


int WM_window_pixels_x(wmWindow *win)
{
  float f = ANCHOR::GetNativePixelSize((ANCHOR_SystemWindowHandle)win->anchorwin);

  GfVec2f win_size = FormFactory(win->size);

  return (int)(f * GET_X(win_size));
}

int WM_window_pixels_y(wmWindow *win)
{
  float f = ANCHOR::GetNativePixelSize((ANCHOR_SystemWindowHandle)win->anchorwin);

  GfVec2f win_size = FormFactory(win->size);

  return (int)(f * GET_Y(win_size));
}


void wm_quit_with_optional_confirmation_prompt(cContext *C, wmWindow *win)
{
  wmWindow *win_ctx = CTX_wm_window(C);

  Stage stage = CTX_data_stage(C);
  UserDef *uprefs = CTX_data_prefs(C);

  /* The popup will be displayed in the context window which may not be set
   * here (this function gets called outside of normal event handling loop). */
  CTX_wm_window_set(C, win);

  bool show_save = FormFactory(uprefs->showsave);

  if (show_save)
  {
    if (stage->GetRootLayer()->IsDirty())
    {
      wm_window_raise(win);
      wm_confirm_quit(C);
    }
    else
    {
      wm_exit_schedule_delayed(C);
    }
  }
  else
  {
    wm_exit_schedule_delayed(C);
  }

  CTX_wm_window_set(C, win_ctx);
}


/* this is event from anchor, or exit-covah op */
void wm_window_close(cContext *C, wmWindowManager *wm, wmWindow *win)
{
  Stage stage = CTX_data_stage(C);

  SdfPath other_hash;

  /* First check if there is another main window remaining. */
  UNIVERSE_FOR_ALL(win_other, wm->windows)
  {
    if (VALUE(win_other) != win && VALUE(win_other)->parent == NULL)
    {
      other_hash = HASH(win_other);
      break;
    }
  }

  if (win->parent == NULL)
  {
    wm_quit_with_optional_confirmation_prompt(C, win);
    return;
  }

  /* Close child windows */
  UNIVERSE_FOR_ALL(iter_win, wm->windows)
  {
    if (VALUE(iter_win)->parent == win)
    {
      wm_window_close(C, wm, VALUE(iter_win));
    }
  }

  /** Remove Window From HashMap */
  wm->windows.erase(win->path);

  /** Remove All Areas from Screens. */
  if (win->prims.screen)
  {
    SdfPathVector areas;
    win->prims.screen->areas_rel.GetTargets(&areas);
    UNIVERSE_FOR_ALL(area, areas)
    {
      UsdPrim areaprim = stage->GetPrimAtPath(area.GetPrimPath());
      areaprim.Unload();
    }

    SdfPathVector screens;
    win->prims.workspace->screen_rel.GetTargets(&screens);
    UNIVERSE_FOR_ALL(screen, screens)
    {
      UsdPrim screenprim = stage->GetPrimAtPath(screen.GetPrimPath());
      screenprim.Unload();
    }
  }

  /** Null out C. */
  if (CTX_wm_window(C) == win)
  {
    CTX_wm_window_set(C, NULL);
  }

  delete win;
}


wmWindow *wm_window_copy(cContext *C,
                         wmWindowManager *wm,
                         wmWindow *win_src,
                         const bool duplicate_layout,
                         const bool child)
{
  const bool is_dialog = ANCHOR::IsDialogWindow((ANCHOR_SystemWindowHandle)win_src->anchorwin);
  wmWindow *win_parent = (child) ? win_src : win_src->parent;

  /* ----- */

  /**
   * Create Window. */
  wmWindow *win_dst = new wmWindow(C, win_parent, SdfPath("Child"));
  wm->windows.insert(std::make_pair(win_dst->path, win_dst));

  /**
   * Dialogs may have a child window as parent.
   * Otherwise, a child must not be a parent too. */
  win_dst->parent = (!is_dialog && win_parent && win_parent->parent) ? win_parent->parent : win_parent;

  /* ----- */

  WorkSpace *workspace = win_src->prims.workspace;

  GfVec2f win_srcpos = FormFactory(win_src->pos);
  GfVec2f win_srcsize = FormFactory(win_src->size);

  win_dst->pos.Set(GfVec2f(GET_X(win_srcpos) + 10, GET_Y(win_srcpos)));
  win_dst->size.Set(GfVec2f(GET_X(win_srcsize), GET_Y(win_srcsize)));

  win_dst->scene = win_src->scene;
  win_dst->workspace_rel.AddTarget(workspace->path);

  return win_dst;
}


wmWindow *wm_window_copy_test(cContext *C,
                              wmWindow *win_src,
                              const bool duplicate_layout,
                              const bool child)
{
  Main *cmain = CTX_data_main(C);
  wmWindowManager *wm = CTX_wm_manager(C);

  wmWindow *win_dst = wm_window_copy(C, wm, win_src, duplicate_layout, child);

  WM_check(C);

  if (win_dst->anchorwin)
  {
    WM_event_add_notifier_ex(wm, CTX_wm_window(C), NC_WINDOW | NA_ADDED, NULL);
    return win_dst;
  }
  wm_window_close(C, wm, win_dst);
  return NULL;
}


static int wm_window_close_exec(cContext *C, wmOperator *UNUSED(op))
{
  wmWindowManager *wm = CTX_wm_manager(C);
  wmWindow *win = CTX_wm_window(C);
  wm_window_close(C, wm, win);
  return OPERATOR_FINISHED;
}


static int wm_window_new_exec(cContext *C, wmOperator *UNUSED(op))
{
  Stage stage = CTX_data_stage(C);
  wmWindow *win_src = CTX_wm_window(C);

  GfVec2f win_size = FormFactory(win_src->size);
  TfToken win_align = FormFactory(win_src->alignment);
  SdfPathVector win_areas = FormFactory(win_src->prims.screen->areas_rel);
  SdfAssetPath win_icon = FormFactory(win_src->icon);

  TfToken spacetype;
  UNIVERSE_FOR_ALL(sdf_area, win_areas)
  {
    auto area = ScrArea::Get(stage, sdf_area.GetPrimPath());
    UsdAttribute type = area.GetSpacetypeAttr();

    TfToken possibletype;
    type.Get(&possibletype);

    spacetype = wm_verify_spacetype(possibletype);
  }

  bool ok = (WM_window_open(C,
                            IFACE_("Covah"),
                            CHARALL(win_icon.GetAssetPath()),
                            0,
                            0,
                            GET_X(win_size) * 0.95f,
                            GET_Y(win_size) * 0.9f,
                            spacetype,
                            win_align,
                            false,
                            false) != NULL);

  return ok ? OPERATOR_FINISHED : OPERATOR_CANCELLED;
}


void WM_anchor_init(cContext *C)
{
  /* Event handle of anchor stack. */
  ANCHOR_EventConsumerHandle consumer;

  if (C != NULL)
  {
    consumer = ANCHOR_CreateEventConsumer(anchor_event_proc, C);
  }

  if (!anchor_system)
  {
    anchor_system = ANCHOR_CreateSystem();
  }

  if (C != NULL)
  {
    ANCHOR::AddEventConsumer(anchor_system, consumer);
  }
}


void WM_window_process_events(cContext *C)
{
  bool has_event = ANCHOR::ProcessEvents(anchor_system, false);

  if (has_event)
  {
    ANCHOR::DispatchEvents(anchor_system);
  }

  if ((has_event == false))
  {
    PIL_sleep_ms(5);
  }
}


void WM_window_swap_buffers(wmWindow *win)
{
  ANCHOR::SwapChain((ANCHOR_SystemWindowHandle)win->anchorwin);
}


/**
 *  -----  The Window Operators. ----- */


static int wm_window_new_main_exec(cContext *C, wmOperator *UNUSED(op))
{
  wmWindow *win_src = CTX_wm_window(C);

  bool ok = (wm_window_copy_test(C, win_src, true, false) != NULL);

  return ok ? OPERATOR_FINISHED : OPERATOR_CANCELLED;
}


static int wm_window_fullscreen_toggle_exec(cContext *C, wmOperator *UNUSED(op))
{
  wmWindow *window = CTX_wm_window(C);

  eAnchorWindowState state = ANCHOR::GetWindowState((ANCHOR_SystemWindowHandle)window->anchorwin);
  if (state != ANCHOR_WindowStateFullScreen)
  {
    ANCHOR::SetWindowState((ANCHOR_SystemWindowHandle)window->anchorwin, ANCHOR_WindowStateFullScreen);
  }
  else
  {
    ANCHOR::SetWindowState((ANCHOR_SystemWindowHandle)window->anchorwin, ANCHOR_WindowStateNormal);
  }

  return OPERATOR_FINISHED;
}


static bool wm_operator_winactive(cContext *C)
{
  if (CTX_wm_window(C) == NULL)
  {
    return 0;
  }
  return 1;
}


static bool wm_operator_winactive_normal(cContext *C)
{
  wmWindow *win = CTX_wm_window(C);

  if (win == NULL)
  {
    return 0;
  }

  if (!(win->prims.screen))
  {
    return 0;
  }

  // TfToken alignment;
  // win->prims.screen->align.Get(&alignment);
  // if (!(alignment == UsdUITokens->none)) {
  //   return 0;
  // }

  return 1;
}


static int wm_exit_covah_exec(cContext *C, wmOperator *UNUSED(op))
{
  wm_exit_schedule_delayed(C);
  return OPERATOR_FINISHED;
}


static int wm_exit_covah_invoke(cContext *C, wmOperator *UNUSED(op), wmEvent *UNUSED(event))
{
  UserDef *uprefs = CTX_data_prefs(C);

  bool prompt_save = FormFactory(uprefs->showsave);

  if (prompt_save)
  {
    wm_quit_with_optional_confirmation_prompt(C, CTX_wm_window(C));
  }
  else
  {
    wm_exit_schedule_delayed(C);
  }
  return OPERATOR_FINISHED;
}


static void WM_OT_window_close(wmOperatorType *ot)
{
  ot->name = "Close Window";
  ot->idname = IDNAME(WM_OT_window_close);
  ot->description = "Close the current window";

  ot->exec = wm_window_close_exec;
  ot->poll = wm_operator_winactive;
}


static void WM_OT_window_new(wmOperatorType *ot)
{
  ot->name = "New Window";
  ot->idname = IDNAME(WM_OT_window_new);
  ot->description = "Create a new window";

  ot->exec = wm_window_new_exec;
  ot->poll = wm_operator_winactive_normal;
}


static void WM_OT_window_new_main(wmOperatorType *ot)
{
  ot->name = "New Main Window";
  ot->idname = IDNAME(WM_OT_window_new_main);
  ot->description = "Create a new main window with its own workspace and scene selection";

  ot->exec = wm_window_new_main_exec;
  ot->poll = wm_operator_winactive_normal;
}


static void WM_OT_window_fullscreen_toggle(wmOperatorType *ot)
{
  ot->name = "Toggle Window Fullscreen";
  ot->idname = IDNAME(WM_OT_window_fullscreen_toggle);
  ot->description = "Toggle the current window fullscreen";

  ot->exec = wm_window_fullscreen_toggle_exec;
  ot->poll = wm_operator_winactive;
}


static void WM_OT_quit_covah(wmOperatorType *ot)
{
  ot->name = "Quit Covah";
  ot->idname = IDNAME(WM_OT_quit_covah);
  ot->description = "Quit Covah";

  ot->invoke = wm_exit_covah_invoke;
  ot->exec = wm_exit_covah_exec;
}


void WM_window_operators_register()
{
  /* ------ */

  WM_operatortype_append(WM_OT_window_close);
  WM_operatortype_append(WM_OT_window_new);
  WM_operatortype_append(WM_OT_window_new_main);
  WM_operatortype_append(WM_OT_window_fullscreen_toggle);
  WM_operatortype_append(WM_OT_quit_covah);

  /* ------ */
}

WABI_NAMESPACE_END