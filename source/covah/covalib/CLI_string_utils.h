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
 * COVAH Library.
 * Gadget Vault.
 */

#pragma once

#include "CLI_api.h"

/* generic strcmp macros */
#if defined(_MSC_VER)
#  define strcasecmp _stricmp
#  define strncasecmp _strnicmp
#endif

/* ------------------------------------------------------------ CLASSIC C STRING UTILITIES ----- */

COVAH_LIB_API
int CLI_strcasecmp(const char *s1, const char *s2);

COVAH_LIB_API
char *CLI_strcasestr(const char *s, const char *find);

COVAH_LIB_API
int CLI_strncasecmp(const char *s1, const char *s2, size_t len);

COVAH_LIB_API
size_t CLI_split_name_num(char *left, int *nr, const char *name, const char delim);

COVAH_LIB_API
char *CLI_strncpy(char *__restrict dst, const char *__restrict src, const size_t maxncpy);

COVAH_LIB_API
size_t CLI_strncpy_utf8_rlen(char *__restrict dst, const char *__restrict src, size_t maxncpy);

COVAH_LIB_API
size_t CLI_strnlen(const char *s, const size_t maxlen);

COVAH_LIB_API
size_t CLI_vsnprintf(char *__restrict buffer, size_t maxncpy, const char *__restrict format, va_list arg);

COVAH_LIB_API
size_t CLI_vsnprintf_rlen(char *__restrict buffer,
                          size_t maxncpy,
                          const char *__restrict format,
                          va_list arg);

COVAH_LIB_API
size_t CLI_snprintf(char *__restrict dst, size_t maxncpy, const char *__restrict format, ...);

COVAH_LIB_API
size_t CLI_snprintf_rlen(char *__restrict dst, size_t maxncpy, const char *__restrict format, ...);

typedef bool (*UniquenameCheckCallback)(void *arg, const char *name);

COVAH_LIB_API
bool CLI_uniquename(std::vector<void *> list, void *vlink, const char *defname, char delim, int name_offset, size_t name_len);

COVAH_LIB_API
bool CLI_uniquename_cb(UniquenameCheckCallback unique_check,
                       void *arg,
                       const char *defname,
                       char delim,
                       char *name,
                       size_t name_len);

#define STRNCPY(dst, src) CLI_strncpy(dst, src, ARRAY_SIZE(dst))
#define STRNCPY_RLEN(dst, src) CLI_strncpy_rlen(dst, src, ARRAY_SIZE(dst))
#define SNPRINTF(dst, format, ...) CLI_snprintf(dst, ARRAY_SIZE(dst), format, __VA_ARGS__)
#define SNPRINTF_RLEN(dst, format, ...) \
  CLI_snprintf_rlen(dst, ARRAY_SIZE(dst), format, __VA_ARGS__)
#define STR_CONCAT(dst, len, suffix) \
  len += CLI_strncpy_rlen(dst + len, suffix, ARRAY_SIZE(dst) - len)
#define STR_CONCATF(dst, len, format, ...) \
  len += CLI_snprintf_rlen(dst + len, ARRAY_SIZE(dst) - len, format, __VA_ARGS__)

#define STREQ(a, b) (strcmp(a, b) == 0)
#define STRCASEEQ(a, b) (strcasecmp(a, b) == 0)
#define STREQLEN(a, b, n) (strncmp(a, b, n) == 0)
#define STRCASEEQLEN(a, b, n) (strncasecmp(a, b, n) == 0)

#define STRPREFIX(a, b) (strncmp((a), (b), strlen(b)) == 0)

/* ------------------------------------------------------ MODERN CXX STD::STRING UTILITIES ----- */

COVAH_LIB_API
std::string CLI_str_CapSpaceAmmender(std::string str);

COVAH_LIB_API
std::string CLI_str_UpperCamel(std::string str);
