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
 * @file UNI_universe.h
 * @ingroup UNI
 * The @a central foundation for @a all data access.
 */

#include "UNI_universe.h"

#include <wabi/base/tf/instantiateSingleton.h>
#include <wabi/base/tf/registryManager.h>
#include <wabi/base/tf/type.h>

WABI_NAMESPACE_USING

TF_INSTANTIATE_SINGLETON(UNIVERSE);

UNIVERSE &UNIVERSE::GetInstance()
{
  return TfSingleton<This>::GetInstance();
}

UNIVERSE::UNIVERSE()
{
  TfSingleton<This>::SetInstanceConstructed(*this);
}

TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<UNIVERSE>();
}