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
 * Creator.
 * Creating Chaos.
 */

#include "environment.h"

#include <fstream>
#include <string>

#ifdef __GNUC__
#  include <cstdlib>
#endif /*__GNUC__ */

#include <wabi/base/arch/systemInfo.h>
#include <wabi/base/tf/stringUtils.h>
#include <wabi/wabi.h>

/**
 * ENVIRONMENT: VARIABLES
 * Setup our environment according
 * to our installation. */

WABI_NAMESPACE_BEGIN

void CREATOR_covah_env_init()
{
  static std::string EXE_PATH = TfGetPathName(ArchGetExecutablePath());
}

WABI_NAMESPACE_END