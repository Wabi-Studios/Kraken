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
 * KRAKEN Library.
 * Gadget Vault.
 */

#pragma once

#include "CLI_api.h"
#include "CLI_assert.h"
#include "CLI_string_utils.h"

#include <cstring>
#include <string>

WABI_NAMESPACE_BEGIN

bool CLI_path_extension_check(const char *str, const char *ext);
bool CLI_path_extension_check_n(const char *str, ...);
bool CLI_path_extension_check_array(const std::string &str, const char **ext_array);

bool CLI_has_pixar_extension(const std::string &str);

WABI_NAMESPACE_END