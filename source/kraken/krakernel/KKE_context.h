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
 * KRAKEN Kernel.
 * Purple Underground.
 */

#pragma once

#include "KKE_api.h"
#include "USD_object.h"

#include <wabi/usd/usd/common.h>

WABI_NAMESPACE_BEGIN

typedef UsdStageRefPtr Stage;

struct ScrArea;
struct Main;
struct ARegion;
struct Scene;
struct kScreen;
struct UserDef;
struct wmWindowManager;
struct wmWindow;
struct WorkSpace;

struct kContext;
struct kContextDataResult;

enum
{
  CTX_DATA_TYPE_POINTER = 0,
  CTX_DATA_TYPE_COLLECTION,
};

/**
 * Kraken Context:
 *  - Creation.
 *  - Destruction. */

kContext *CTX_create(void);
void CTX_free(kContext *C);

/**
 * need to store if python
 * is initialized or not */
bool CTX_py_init_get(kContext *C);
void CTX_py_init_set(kContext *C, bool value);
void *CTX_py_dict_get(const kContext *C);
void CTX_data_pointer_set_ptr(kContextDataResult *result, const KrakenPRIM *ptr);
void CTX_data_type_set(kContextDataResult *result, short type);
void CTX_data_list_add_ptr(kContextDataResult *result, const KrakenPRIM *ptr);

/**
 * Kraken Context Getters:
 *  - System Main.
 *  - WindowManager.
 *  - Scene data.
 *  - Stage data. */

Main *CTX_data_main(const kContext *C);
wmWindowManager *CTX_wm_manager(const kContext *C);
wmWindow *CTX_wm_window(const kContext *C);
WorkSpace *CTX_wm_workspace(const kContext *C);
kScreen *CTX_wm_screen(const kContext *C);
ScrArea *CTX_wm_area(const kContext *C);
ARegion *CTX_wm_region(const kContext *C);
ARegion *CTX_wm_menu(const kContext *C);
Scene *CTX_data_scene(const kContext *C);
Stage CTX_data_stage(const kContext *C);
UserDef *CTX_data_prefs(const kContext *C);

/**
 * Kraken Context Setters:
 *  - System Main.
 *  - WindowManager.
 *  - Scene data.
 *  - Stage data. */

void CTX_data_main_set(kContext *C, Main *kmain);
void CTX_wm_manager_set(kContext *C, wmWindowManager *wm);
void CTX_wm_screen_set(kContext *C, kScreen *screen);
void CTX_wm_area_set(kContext *C, ScrArea *area);
void CTX_wm_region_set(kContext *C, ARegion *region);
void CTX_wm_menu_set(kContext *C, ARegion *menu);
void CTX_wm_window_set(kContext *C, wmWindow *win);
void CTX_data_scene_set(kContext *C, Scene *cscene);
void CTX_data_prefs_set(kContext *C, UserDef *uprefs);

void CTX_wm_operator_poll_msg_clear(kContext *C);
void CTX_wm_operator_poll_msg_set(kContext *C, const char *msg);

typedef int (*kContextDataCallback)(const kContext *C,
                                    const char *member,
                                    kContextDataResult *result);

struct kContextPollMsgParams
{
  char *(*get_fn)(kContext *C, void *user_data);
  void (*free_fn)(kContext *C, void *user_data);
  void *user_data;
};

WABI_NAMESPACE_END