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

#pragma once

#include "UNI_path_defaults.h"

#include "CLI_utildefines.h"

#include <wabi/usd/usd/attribute.h>

#include <cstddef>
#include <iterator>

#if defined(WABI_STATIC)
#  define COVAH_UNIVERSE_API
#  define COVAH_UNIVERSE_API_TEMPLATE_CLASS(...)
#  define COVAH_UNIVERSE_API_TEMPLATE_STRUCT(...)
#  define COVAH_UNIVERSE_LOCAL
#else
#  if defined(COVAH_UNIVERSE_EXPORTS)
#    define COVAH_UNIVERSE_API ARCH_EXPORT
#    define COVAH_UNIVERSE_API_TEMPLATE_CLASS(...) ARCH_EXPORT_TEMPLATE(class, __VA_ARGS__)
#    define COVAH_UNIVERSE_API_TEMPLATE_STRUCT(...) ARCH_EXPORT_TEMPLATE(struct, __VA_ARGS__)
#  else
#    define COVAH_UNIVERSE_API ARCH_IMPORT
#    define COVAH_UNIVERSE_API_TEMPLATE_CLASS(...) ARCH_IMPORT_TEMPLATE(class, __VA_ARGS__)
#    define COVAH_UNIVERSE_API_TEMPLATE_STRUCT(...) ARCH_IMPORT_TEMPLATE(struct, __VA_ARGS__)
#  endif
#  define COVAH_UNIVERSE_LOCAL ARCH_HIDDEN
#endif

#define COVAH_DECLARE_STATIC_TOKEN(x) const TfToken x
#define COVAH_DEFINE_STATIC_TOKEN(y) y(STRINGIFY_APPEND("", y), TfToken::Immortal)
#define IDNAME(z) COVAH_OPERATOR_TOKENS->z

#define COVAH_UNIVERSE_CREATE_CHILD(i) Define(CTX_data_stage(i), prim->path.AppendPath(stagepath))
#define COVAH_UNIVERSE_CREATE(g) Define(CTX_data_stage(g), stagepath)
#define COVAH_PRIM_OPERATOR_CREATE(x, y) CTX_data_stage(x)->DefinePrim(SdfPath(COVAH_PATH_DEFAULTS::COVAH_OPERATORS).AppendPath(SdfPath(y)))

#define GET_X(x) x[0]
#define GET_Y(y) y[1]
#define GET_Z(z) z[2]
#define GET_W(w) w[3]

#define VEC2_SET(vx2, vx1x, vx2y) vx2->Set(vx1x, vx2y)
#define VEC3_SET(vx3, vx1x, vx2y, vx3z) vx3->Set(vx1x, vx2y, vx3z)
#define VEC4_SET(vx4, vx1x, vx2y, vx3z, vx4w) vx4->Set(vx1x, vx2y, vx3z, vx4w)
#define SET_VEC2(vx2, vx1x, vx2y) vx2.Set(vx1x, vx2y)
#define SET_VEC3(vx3, vx1x, vx2y, vx3z) vx3.Set(vx1x, vx2y, vx3z)
#define SET_VEC4(vx4, vx1x, vx2y, vx3z, vx4w) vx4.Set(vx1x, vx2y, vx3z, vx4w)

#define UNIVERSE_INSERT_WINDOW(m, h, v) m->windows.insert(std::make_pair(h, v))
#define HASH(x) x.first
#define VALUE(y) y.second
#define UNIVERSE_FOR_ALL(iter, c) for (const auto &iter : c)
#define UNIVERSE_MUTABLE_FOR_ALL(iter, c) for (auto &iter : c)

#define FILE_MAX 1024

#define TIMECODE_DEFAULT UsdTimeCode::Default()