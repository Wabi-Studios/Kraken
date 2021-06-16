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
 * COVAH Kernel.
 * Purple Underground.
 */

#include "CKE_context.h"
#include "CKE_main.h"
#include "CKE_version.h"

#include "UNI_window.h"

#include <wabi/base/tf/mallocTag.h>
#include <wabi/usd/usd/attribute.h>

WABI_NAMESPACE_USING

struct cContext {

  cContext() = default;

  int thread;

  /* windowmanager context */
  struct {
    struct wmWindowManager *manager;
    struct wmWindow *window;

    const char *operator_poll_msg;
  } wm;

  /* data context */
  struct {
    struct Main *main;
    struct Scene *scene;
  } data;
};

/**
 * Main CTX Creation. */
cContext *CTX_create(void)
{
  TfAutoMallocTag2 tag("cContext", "CTX_create");

  cContext *C = new cContext();

  return C;
}

/**
 * Main CTX Deletion. */
void CTX_free(cContext *C)
{
  TfAutoMallocTag2 tag("cContext", "CTX_free");

  delete C;
}

/**
 * Getters. */

Main *CTX_data_main(const cContext *C)
{
  return C->data.main;
}

wmWindowManager *CTX_wm_manager(const cContext *C)
{
  return C->wm.manager;
}

wmWindow *CTX_wm_window(const cContext *C)
{
  return C->wm.window;
}

Scene *CTX_data_scene(const cContext *C)
{
  return C->data.scene;
}

/**
 * Setters. */

void CTX_data_main_set(cContext *C, Main *cmain)
{
  C->data.main = cmain;
}

void CTX_wm_manager_set(cContext *C, wmWindowManager *wm)
{
  C->wm.manager = wm;
}

void CTX_wm_window_set(cContext *C, wmWindow *win)
{
  C->wm.window = win;
  if (win) {
    C->data.scene = win->scene;
  }

  // C->wm.workspace = (win) ? BKE_workspace_active_get(win->workspace_hook) : NULL;
  // C->wm.screen = (win) ? BKE_workspace_active_screen_get(win->workspace_hook) : NULL;
  // C->wm.area = NULL;
  // C->wm.region = NULL;
}

void CTX_data_scene_set(cContext *C, Scene *cscene)
{
  C->data.scene = cscene;
}