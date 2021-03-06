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

#include "USD_window.h"

#include "ANCHOR_api.h"

#include "KKE_context.h"

#include <wabi/base/tf/debug.h>
#include <wabi/wabi.h>

WABI_NAMESPACE_BEGIN

void WM_cursor_grab_enable(wmWindow *win, int wrap, bool hide, int bounds[4]);

void WM_cursor_position_from_anchor(wmWindow *win, int *x, int *y);
void WM_cursor_position_to_anchor(wmWindow *win, int *x, int *y);

WABI_NAMESPACE_END