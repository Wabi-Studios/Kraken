//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef WABI_BASE_ARCH_THREADS_H
#define WABI_BASE_ARCH_THREADS_H

/// \file arch/threads.h
/// \ingroup group_arch_Multithreading
/// Architecture-specific thread function calls.

#include "wabi/wabi.h"
#include "wabi/base/arch/api.h"

#include <thread>

WABI_NAMESPACE_BEGIN

/// Return true if the calling thread is the main thread, false otherwise.
/// \ingroup group_arch_Multithreading
ARCH_API bool ArchIsMainThread();

/// Return the std::thread_id for the thread arch considers to be the "main"
/// thread.
ARCH_API std::thread::id ArchGetMainThreadId();

WABI_NAMESPACE_END

#endif  // WABI_BASE_ARCH_THREADS_H
