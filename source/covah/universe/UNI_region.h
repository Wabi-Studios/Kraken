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

#pragma once

/**
 * @file
 * Universe.
 * Set the Stage.
 */

#include "UNI_context.h"
#include "UNI_screen.h"

#include "CKE_context.h"

#include <wabi/usd/usdUI/area.h>

WABI_NAMESPACE_BEGIN

enum eRegionType
{
  RGN_TYPE_WINDOW = 0,
  RGN_TYPE_HEADER = 1,
  RGN_TYPE_CHANNELS = 2,
  RGN_TYPE_TEMPORARY = 3,
  RGN_TYPE_UI = 4,
  RGN_TYPE_TOOLS = 5,
  RGN_TYPE_TOOL_PROPS = 6,
  RGN_TYPE_PREVIEW = 7,
  RGN_TYPE_HUD = 8,
  /* Region to navigate the main region from (RGN_TYPE_WINDOW). */
  RGN_TYPE_NAV_BAR = 9,
  /* A place for buttons to trigger execution of something that was set up in other regions. */
  RGN_TYPE_EXECUTE = 10,
  RGN_TYPE_FOOTER = 11,
  RGN_TYPE_TOOL_HEADER = 12,
};


#define RGN_TYPE_IS_HEADER_ANY(regiontype) \
  (((1 << (regiontype)) & \
    ((1 << RGN_TYPE_HEADER) | 1 << (RGN_TYPE_TOOL_HEADER) | (1 << RGN_TYPE_FOOTER))) != 0)


struct CovahRegion : public UsdUIArea, public CovahObject
{
  SdfPath path;

  UsdAttribute name;
  UsdAttribute spacetype;
  UsdAttribute icon;
  UsdAttribute pos;
  UsdAttribute size;

  eRegionType regiontype;

  inline CovahRegion(const cContext &C, const cScreen &prim, const SdfPath &stagepath);
};

CovahRegion::CovahRegion(const cContext &C, const cScreen &prim, const SdfPath &stagepath)
  : UsdUIArea(COVAH_UNIVERSE_CREATE_CHILD(C)),
    path(GetPath()),
    name(CreateNameAttr()),
    spacetype(CreateSpacetypeAttr()),
    icon(CreateIconAttr()),
    pos(CreatePosAttr()),
    size(CreateSizeAttr()),
    regiontype(RGN_TYPE_WINDOW)
{}

WABI_NAMESPACE_END