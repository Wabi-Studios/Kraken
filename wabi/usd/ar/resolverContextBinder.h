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
#ifndef WABI_USD_AR_RESOLVER_CONTEXT_BINDER_H
#define WABI_USD_AR_RESOLVER_CONTEXT_BINDER_H

/// \file ar/resolverContextBinder.h

#include "wabi/wabi.h"
#include "wabi/usd/ar/api.h"
#include "wabi/usd/ar/resolverContext.h"
#include "wabi/base/vt/value.h"

WABI_NAMESPACE_BEGIN

class ArResolver;

/// \class ArResolverContextBinder
///
/// Helper object for managing the binding and unbinding of
/// ArResolverContext objects with the asset resolver.
///
/// \see \ref ArResolver_context "Asset Resolver Context Operations"
class ArResolverContextBinder
{
 public:

  /// Bind the given \p context with the asset resolver.
  ///
  /// Calls ArResolver::BindContext on the configured asset resolver
  /// and saves the bindingData populated by that function.
  AR_API
  ArResolverContextBinder(const ArResolverContext &context);

  /// Bind the given \p context to the given \p assetResolver.
  ///
  /// Calls ArResolver::BindContext on the given \p assetResolver
  /// and saves the bindingData populated by that function.
  AR_API
  ArResolverContextBinder(ArResolver *assetResolver, const ArResolverContext &context);

  /// Unbinds the context specified in the constructor of this
  /// object from the asset resolver.
  ///
  /// Calls ArResolver::UnbindContext on the asset resolver that was
  /// bound to originally, passing the saved bindingData to that function.
  AR_API
  ~ArResolverContextBinder();

 private:

  ArResolver *_resolver;
  ArResolverContext _context;
  VtValue _bindingData;
};

WABI_NAMESPACE_END

#endif  // WABI_USD_AR_RESOLVER_CONTEXT_BINDER_H
