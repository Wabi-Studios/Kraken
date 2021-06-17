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

#pragma once

#include "CKE_api.h"

#include <wabi/usd/usd/common.h>

WABI_NAMESPACE_BEGIN

typedef UsdStageRefPtr Stage;

struct CovahMain;
struct CovahScene;
struct CovahScreen;
struct CovahWindowManager;
struct CovahWindow;
struct CovahWorkspace;
struct CovahContext;

TF_DECLARE_WEAK_AND_REF_PTRS(CovahMain);
TF_DECLARE_WEAK_AND_REF_PTRS(CovahScene);
TF_DECLARE_WEAK_AND_REF_PTRS(CovahScreen);
TF_DECLARE_WEAK_AND_REF_PTRS(CovahWindowManager);
TF_DECLARE_WEAK_AND_REF_PTRS(CovahWindow);
TF_DECLARE_WEAK_AND_REF_PTRS(CovahWorkspace);
TF_DECLARE_WEAK_AND_REF_PTRS(CovahContext);

/**
 * We do not hold pointers to
 * CovahXXX objects. We hold
 * reference pointers to the
 * following types: */

/* clang-format off */
typedef CovahMainRefPtr          /* Use -> */ Main;
typedef CovahSceneRefPtr         /* Use -> */ Scene;
typedef CovahScreenRefPtr        /* Use -> */ cScreen;
typedef CovahWindowManagerRefPtr /* Use -> */ wmWindowManager;
typedef CovahWindowRefPtr        /* Use -> */ wmWindow;
typedef CovahWorkspaceRefPtr     /* Use -> */ Workspace;
typedef CovahContextRefPtr       /* Use -> */ cContext;
/* clang-format on */

/**
 * Covah Context:
 *  - Creation.
 *  - Destruction. */

cContext CTX_create(void);
void CTX_free(cContext C);

/**
 * Covah Context Getters:
 *  - System Main.
 *  - WindowManager.
 *  - Scene data.
 *  - Stage data. */

Main CTX_data_main(const cContext &C);
wmWindowManager CTX_wm_manager(const cContext &C);
wmWindow CTX_wm_window(const cContext &C);
Scene CTX_data_scene(const cContext &C);
Stage CTX_data_stage(const cContext &C);

/**
 * Covah Context Setters:
 *  - System Main.
 *  - WindowManager.
 *  - Scene data.
 *  - Stage data. */

void CTX_data_main_set(cContext C, Main cmain);
void CTX_wm_manager_set(cContext C, wmWindowManager wm);
void CTX_wm_window_set(cContext C, wmWindow win);
void CTX_data_scene_set(cContext C, Scene cscene);

WABI_NAMESPACE_END