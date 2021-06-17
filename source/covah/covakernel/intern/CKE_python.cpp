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

#include "CKE_api.h" /* Own Include. */
#include "CKE_main.h"

#include "wpy/WPY_init_exit.h"

#include <wabi/base/tf/stringUtils.h>
#include <wabi/wabi.h>

WABI_NAMESPACE_BEGIN

void CKE_covah_python_init()
{
  const std::string sys_paths = TfStringCatPaths(G.main->datafiles_path, "../python/lib/");

  WPY_python_init(sys_paths);
}

WABI_NAMESPACE_END