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
#ifndef WABI_USD_AR_PY_RESOLVER_CONTEXT_H
#define WABI_USD_AR_PY_RESOLVER_CONTEXT_H

/// \file ar/pyResolverContext.h

#include "wabi/wabi.h"
#include "wabi/usd/ar/ar.h"

#define INCLUDE_AR_PY_RESOLVER_CONTEXT

#if AR_VERSION == 1
#  include "wabi/usd/ar/pyResolverContext_v1.h"
#elif AR_VERSION == 2
#  include "wabi/usd/ar/pyResolverContext_v2.h"
#else
#  error Unhandled AR_VERSION
#endif

#undef INCLUDE_AR_PY_RESOLVER_CONTEXT

#endif  // WABI_USD_AR_PY_RESOLVER_CONTEXT_H
