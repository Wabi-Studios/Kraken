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
 * KRAKEN Python.
 * It Bites.
 */

#include <Python.h>

#include "KPY_api.h"

WABI_NAMESPACE_BEGIN

struct PyC_StringEnumItems
{
  int value;
  const char *id;
};
struct PyC_StringEnum
{
  const struct PyC_StringEnumItems *items;
  int value_found;
};

/* Integer parsing (with overflow checks), -1 on error. */
int PyC_Long_AsBool(PyObject *value);
int8_t PyC_Long_AsI8(PyObject *value);
int16_t PyC_Long_AsI16(PyObject *value);
#if 0 /* inline */
int32_t PyC_Long_AsI32(PyObject *value);
int64_t PyC_Long_AsI64(PyObject *value);
#endif

uint8_t PyC_Long_AsU8(PyObject *value);
uint16_t PyC_Long_AsU16(PyObject *value);
uint32_t PyC_Long_AsU32(PyObject *value);
#if 0 /* inline */
uint64_t PyC_Long_AsU64(PyObject *value);
#endif

PyObject *PyC_Err_Format_Prefix(PyObject *exception_type_prefix, const char *format, ...);
PyObject *PyC_Err_SetString_Prefix(PyObject *exception_type_prefix, const char *str);

int PyC_ParseStringEnum(PyObject *o, void *p);
int PyC_ParseBool(PyObject *o, void *p);

PyObject *PyC_UnicodeFromByteAndSize(const char *str, Py_ssize_t size);
PyObject *PyC_UnicodeFromByte(const char *str);

WABI_NAMESPACE_END