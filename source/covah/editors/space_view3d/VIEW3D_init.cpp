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
 * Editors.
 * Tools for Artists.
 */

#include "ANCHOR_api.h"

#include "ED_tokens.h"

#include "ED_view3d.h"

#include "CKE_main.h"

WABI_NAMESPACE_BEGIN

void ED_view3d_init_engine(const SdfPath &root, bool &reset)
{
  TF_UNUSED(root);

  if (ARCH_UNLIKELY(reset))
  {
    ANCHOR::GetEngineApollo().reset(new UsdApolloEngine(ANCHOR::GetPixarDriver()));
    reset = false;
  }
}

WABI_NAMESPACE_END