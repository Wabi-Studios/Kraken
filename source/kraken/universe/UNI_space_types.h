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

#include "UNI_api.h"
#include "UNI_region.h"

WABI_NAMESPACE_BEGIN

enum eFileSelectFileTypes
{
  FILE_TYPE_PIXAR = (1 << 2),
  FILE_TYPE_PIXAR_BACKUP = (1 << 3),
  FILE_TYPE_IMAGE = (1 << 4),
  FILE_TYPE_MOVIE = (1 << 5),
  FILE_TYPE_PYSCRIPT = (1 << 6),
  FILE_TYPE_FTFONT = (1 << 7),
  FILE_TYPE_SOUND = (1 << 8),
  FILE_TYPE_TEXT = (1 << 9),
  FILE_TYPE_ARCHIVE = (1 << 10),
  /** represents folders for filtering */
  FILE_TYPE_FOLDER = (1 << 11),
  FILE_TYPE_BTX = (1 << 12),
  FILE_TYPE_COLLADA = (1 << 13),
  /** from filter_glob operator property */
  FILE_TYPE_OPERATOR = (1 << 14),
  FILE_TYPE_APPLICATIONBUNDLE = (1 << 15),
  FILE_TYPE_ALEMBIC = (1 << 16),
  /** For all kinds of recognized import/export formats. No need for specialized types. */
  FILE_TYPE_OBJECT_IO = (1 << 17),
  FILE_TYPE_BLENDER = (1 << 18),
  FILE_TYPE_VOLUME = (1 << 19),

  FILE_TYPE_ASSET = (1 << 28),
  /** An FS directory (i.e. S_ISDIR on its path is true). */
  FILE_TYPE_DIR = (1 << 30),
};

#define SPACE_TYPE_ANY -1

enum eSpaceLinkFlag
{
  SPACE_FLAG_TYPE_TEMPORARY = (1 << 0),
  SPACE_FLAG_TYPE_WAS_ACTIVE = (1 << 1),
};

struct SpaceLink
{
  /** Storage of regions for inactive spaces. */
  std::vector<ARegion *> regions;
  char spacetype;
  char link_flag;

  SpaceLink()
    : regions(EMPTY),
      spacetype(VALUE_ZERO),
      link_flag(VALUE_ZERO)
  {}
};

WABI_NAMESPACE_END