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

#include <wabi/usd/usd/prim.h>

WABI_NAMESPACE_BEGIN

enum
{
  OPERATOR_RUNNING_MODAL = (1 << 0),
  OPERATOR_CANCELLED = (1 << 1),
  OPERATOR_FINISHED = (1 << 2),
  OPERATOR_PASS_THROUGH = (1 << 3),
  OPERATOR_HANDLED = (1 << 4),
  OPERATOR_INTERFACE = (1 << 5),
};

struct wmOperatorType
{
  /** Text for UI, undo. */
  const char *name;
  /** Unique identifier. */
  const char *idname;
  /** Use for tool-tips and Python docs. */
  const char *description;

  int (*exec)(const cContext &C, UsdAttribute *op) ATTR_WARN_UNUSED_RESULT;

  bool (*poll)(const cContext &C) ATTR_WARN_UNUSED_RESULT;

  int (*invoke)(const cContext &C, UsdAttribute *op, const struct wmEvent *) ATTR_WARN_UNUSED_RESULT;
};

WABI_NAMESPACE_END