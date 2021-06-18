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

#include "CLI_utildefines.h"

#include "CKE_context.h"

#if defined(WABI_STATIC)
#  define COVAH_WM_API
#  define COVAH_WM_API_TEMPLATE_CLASS(...)
#  define COVAH_WM_API_TEMPLATE_STRUCT(...)
#  define COVAH_WM_LOCAL
#else
#  if defined(COVAH_WM_EXPORTS)
#    define COVAH_WM_API ARCH_EXPORT
#    define COVAH_WM_API_TEMPLATE_CLASS(...) ARCH_EXPORT_TEMPLATE(class, __VA_ARGS__)
#    define COVAH_WM_API_TEMPLATE_STRUCT(...) ARCH_EXPORT_TEMPLATE(struct, __VA_ARGS__)
#  else
#    define COVAH_WM_API ARCH_IMPORT
#    define COVAH_WM_API_TEMPLATE_CLASS(...) ARCH_IMPORT_TEMPLATE(class, __VA_ARGS__)
#    define COVAH_WM_API_TEMPLATE_STRUCT(...) ARCH_IMPORT_TEMPLATE(struct, __VA_ARGS__)
#  endif
#  define COVAH_WM_LOCAL ARCH_HIDDEN
#endif

WABI_NAMESPACE_BEGIN

struct wmEvent
{
  struct wmEvent *next, *prev;

  /** Event code itself (short, is also in key-map). */
  short type;
  /** Press, release, scroll-value. */
  short val;
  /** Mouse pointer position, screen coord. */
  int x, y;
  /** Region relative mouse position. */
  int mval[2];
  /**
   * From, anchor if utf8 is enabled for the platform.
   */
  char utf8_buf[6];
  /** From anchor, fallback if utf8 isn't set. */
  char ascii;

  /**
   * Generated by auto-repeat, note that this must only ever be set for keyboard events
   * where `ISKEYBOARD(event->type) == true`.
   *
   * See #KMI_REPEAT_IGNORE for details on how key-map handling uses this.
   */
  char is_repeat;

  /** The previous value of `type`. */
  short prevtype;
  /** The previous value of `val`. */
  short prevval;
  /** The time when the key is pressed, see #PIL_check_seconds_timer. */
  double prevclicktime;
  /** The location when the key is pressed (used to enforce drag thresholds). */
  int prevclickx, prevclicky;
  /**
   * The previous value of #wmEvent.x #wmEvent.y,
   * Unlike other previous state variables, this is set on any mouse motion.
   * Use `prevclickx` & `prevclicky` for the value at time of pressing.
   */
  int prevx, prevy;

  /** Modifier states. */
  /** 'oskey' is apple or windows-key, value denotes order of pressed. */
  short shift, ctrl, alt, oskey;
  /** Raw-key modifier (allow using any key as a modifier). */
  short keymodifier;

  /* Custom data. */
  /** Custom data type, stylus, 6dof, see wm_event_types.h */
  short custom;
  short customdatafree;
  int pad2;
  /** Ascii, unicode, mouse-coords, angles, vectors, NDOF data, drag-drop info. */
  void *customdata;

  /**
   * True if the operating system inverted the delta x/y values and resulting
   * `prevx`, `prevy` values, for natural scroll direction.
   * For absolute scroll direction, the delta must be negated again.
   */
  char is_direction_inverted;
};

COVAH_WM_API
void WM_main(const cContext &C);

WABI_NAMESPACE_END