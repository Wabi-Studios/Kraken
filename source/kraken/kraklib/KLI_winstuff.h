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
 * KRAKEN Library.
 * Gadget Vault. */

#ifndef _WIN32
#  error "This include is for Windows only!"
#endif

#include "KLI_math_inline.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <wabi/wabi.h>

#undef rad
#undef rad1
#undef rad2
#undef rad3
#undef vec
#undef rect
#undef rct1
#undef rct2

#undef small

// These definitions are also in KLI_math for simplicity

#if !defined(_USE_MATH_DEFINES)
#  define _USE_MATH_DEFINES
#endif

#define MAXPATHLEN MAX_PATH

/* Defines for using ISO C++ conferment names. */
#if !defined(_MSC_VER) || _MSC_VER < 1900
#  define snprintf _snprintf
#endif

#if defined(_MSC_VER)
#  define R_OK 4
#  define W_OK 2
// not accepted by access() on windows
//#  define X_OK    1
#  define F_OK 0
#endif

typedef unsigned int mode_t;

#ifndef _SSIZE_T_
#  define _SSIZE_T_
/* python uses HAVE_SSIZE_T */
#  ifndef HAVE_SSIZE_T
#    define HAVE_SSIZE_T 1
typedef SSIZE_T ssize_t;
#  endif
#endif

#ifndef S_ISREG
#  define S_ISREG(x) (((x)&_S_IFREG) == _S_IFREG)
#endif
#ifndef S_ISDIR
#  define S_ISDIR(x) (((x)&_S_IFDIR) == _S_IFDIR)
#endif

WABI_NAMESPACE_BEGIN

/* Directory reading compatibility with UNIX. */
struct dirent
{
  int d_ino;
  int d_off;
  unsigned short d_reclen;
  char *d_name;
};

typedef struct __dirstream DIR;

DIR *opendir(const char *path);
struct dirent *readdir(DIR *dp);
int closedir(DIR *dp);
const char *dirname(char *path);

/* Windows utility functions. */
bool KLI_windows_register_pixar_extension(const bool background);
void KLI_windows_get_default_root_dir(char *root_dir);
int KLI_windows_get_executable_dir(char *str);

int uaccess(const char *filename, int mode);

WABI_NAMESPACE_END