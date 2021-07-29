/*
 * Copyright 2021 Pixar. All Rights Reserved.
 *
 * Portions of this file are derived from original work by Pixar
 * distributed with Universal Scene Description, a project of the
 * Academy Software Foundation (ASWF). https://www.aswf.io/
 *
 * Licensed under the Apache License, Version 2.0 (the "Apache License")
 * with the following modification; you may not use this file except in
 * compliance with the Apache License and the following modification:
 * Section 6. Trademarks. is deleted and replaced with:
 *
 * 6. Trademarks. This License does not grant permission to use the trade
 *    names, trademarks, service marks, or product names of the Licensor
 *    and its affiliates, except as required to comply with Section 4(c)
 *    of the License and to reproduce the content of the NOTICE file.
 *
 * You may obtain a copy of the Apache License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the Apache License with the above modification is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 * ANY KIND, either express or implied. See the Apache License for the
 * specific language governing permissions and limitations under the
 * Apache License.
 *
 * Modifications copyright (C) 2020-2021 Wabi.
 */
/**
 * WARNING: DO NOT AIMLESSLY SLAM THOUSANDS OF
 * SYSTEM INCLUDES INTO A SINGLE PRECOMPILED HEADER
 * AND EXPECT IT NOT TO CAUSE ANY SIGNIFICANT BUILD
 * PROBLEMS -- *** HEAP ISSUES GALORE ***
 * -- Fixed by Furby ❤︎ */

#define TF_MAX_ARITY 7
#include "wabi/wabi.h"
#include "wabi/base/arch/defines.h"
#if defined(ARCH_OS_DARWIN)
#  include <mach/mach_time.h>
#endif
#if defined(ARCH_OS_LINUX)
#  include <x86intrin.h>
#endif
#if defined(ARCH_OS_WINDOWS)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#endif

#include <string>
#include <unordered_map>
#include <vector>
#include <stdarg.h>
#include <stdio.h>
#include <fstream>
#include <unordered_map>

#ifdef WITH_PYTHON
#  include <boost/python.hpp>
#  include <boost/python/class.hpp>
#  include <boost/python/converter/from_python.hpp>
#  include <boost/python/def.hpp>
#  include <boost/python/tuple.hpp>
#  if defined(__APPLE__)  // Fix breakage caused by Python's pyport.h.
#    undef tolower
#    undef toupper
#  endif
#endif  // WITH_PYTHON
#ifdef WITH_PYTHON
#  include "wabi/base/tf/pySafePython.h"
#endif  // WITH_PYTHON