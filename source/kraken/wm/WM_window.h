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

#pragma once

#include "WM_api.h"

#include "WM_msgbus.h"
#include "WM_operators.h"

#include "KKE_context.h"

#include <wabi/base/gf/rect2i.h>

WABI_NAMESPACE_BEGIN

typedef void (*wmGenericUserDataFreeFn)(void *data);

struct wmGenericUserData
{
  void *data;
  wmGenericUserDataFreeFn free_fn;
  bool use_free;
};

typedef void (*wmGenericCallbackFn)(kContext *C, void *user_data);

struct wmGenericCallback
{
  wmGenericCallbackFn exec;
  void *user_data;
  wmGenericUserDataFreeFn free_user_data;
};

wmWindow *wm_window_new(kContext *C, wmWindowManager *wm, wmWindow *parent, bool dialog);
wmWindow *WM_window_open(kContext *C,
                         const char *title,
                         const char *icon,
                         int x,
                         int y,
                         int sizex,
                         int sizey,
                         TfToken space_type,
                         TfToken alignment,
                         bool dialog,
                         bool temp);
void wm_window_close(kContext *C, wmWindowManager *wm, wmWindow *win);
void wm_window_make_drawable(wmWindowManager *wm, wmWindow *win);
bool WM_window_is_temp_screen(const wmWindow *win);

void WM_anchor_init(kContext *C);
void WM_anchor_exit(void);

void WM_window_process_events(kContext *C);
void WM_window_swap_buffers(wmWindow *win);

void WM_window_anchorwindows_ensure(wmWindowManager *wm);

/** Operators :: Register */
void WM_window_operators_register();

/** Utils. */
int WM_window_pixels_x(const wmWindow *win);
int WM_window_pixels_y(const wmWindow *win);
bool WM_window_find_under_cursor(wmWindowManager *wm,
                                 wmWindow *win_ignore,
                                 wmWindow *win,
                                 const GfVec2i mval,
                                 wmWindow **r_win,
                                 GfVec2i *r_mval);
void WM_window_screen_rect_calc(const wmWindow *win, GfRect2i *r_rect);

Scene *WM_window_get_active_scene(const wmWindow *win);
WorkSpace *WM_window_get_active_workspace(const wmWindow *win);
kScreen *WM_window_get_active_screen(const wmWindow *win);
WorkSpaceLayout *WM_window_get_active_layout(const wmWindow *win);

void WM_window_set_active_layout(wmWindow *win, WorkSpace *workspace, WorkSpaceLayout *layout);

/** Cleanup. */
void WM_exit_schedule_delayed(const kContext *C);
void WM_quit_with_optional_confirmation_prompt(kContext *C, wmWindow *win);

WABI_NAMESPACE_END