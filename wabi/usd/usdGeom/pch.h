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
// WARNING: THIS FILE IS GENERATED.  DO NOT EDIT.`
//

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

#  include <intrin.h>
#endif
#include <algorithm>
#include <atomic>
#include <bitset>
#include <cfloat>
#include <cinttypes>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <float.h>
#include <functional>
#include <initializer_list>
#include <iosfwd>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <new>
#include <numeric>
#include <set>
#include <sstream>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <thread>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <boost/functional/hash.hpp>
#include <boost/optional.hpp>
#include <boost/shared_array.hpp>
#include <boost/variant.hpp>
#ifdef WITH_PYTHON
#  include <boost/python.hpp>
#  include <boost/python/class.hpp>
#  include <boost/python/implicit.hpp>
#  include <boost/python/operators.hpp>
#  include <boost/python/tuple.hpp>
#  include <boost/python/def.hpp>
#  include <boost/python/stl_iterator.hpp>
#  if defined(__APPLE__)  // Fix breakage caused by Python's pyport.h.
#    undef tolower
#    undef toupper
#  endif
#endif  // WITH_PYTHON
#include <tbb/enumerable_thread_specific.h>
#include <tbb/queuing_rw_mutex.h>
#ifdef WITH_PYTHON
#  include "wabi/base/tf/pySafePython.h"
#endif  // WITH_PYTHON
