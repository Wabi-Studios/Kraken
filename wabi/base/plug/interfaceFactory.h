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
#ifndef WABI_BASE_PLUG_INTERFACE_FACTORY_H
#define WABI_BASE_PLUG_INTERFACE_FACTORY_H

/// \file plug/interfaceFactory.h

#include "wabi/wabi.h"
#include "wabi/base/tf/type.h"

WABI_NAMESPACE_BEGIN

// For use by \c PLUG_REGISTER_INTERFACE_SINGLETON_TYPE.
class Plug_InterfaceFactory
{
 public:

  struct Base : public TfType::FactoryBase
  {
   public:

    virtual void *New() = 0;
  };

  template<class Interface, class Implementation> struct SingletonFactory : public Base
  {
   public:

    virtual void *New()
    {
      static_assert(std::is_abstract<Interface>::value, "Interface type must be abstract.");
      static Implementation impl;
      return static_cast<Interface *>(&impl);
    }
  };
};

/// Defines the \c Interface \c TfType with a factory to return a
/// \c Implementation singleton.  This is suitable for use with
/// \c PlugStaticInterface. \p Interface must be abstract and
/// \p Implementation a concrete subclass of \p Interface.  Note
/// that this is a factory on \c Interface \b not \c Implementation.
///
/// The result of the factory is a singleton instance of \c Implementation
/// and the client of TfType::GetFactory() must not destroy it.
///
/// Clients that want to create instances of types defined in a plugin
/// but not added to the TfType system should create a singleton with
/// factory methods to create those objects.
#define PLUG_REGISTER_INTERFACE_SINGLETON_TYPE(Interface, Implementation)                \
  TF_REGISTRY_FUNCTION(TfType)                                                           \
  {                                                                                      \
    TfType::Define<Interface>()                                                          \
      .SetFactory<Plug_InterfaceFactory::SingletonFactory<Interface, Implementation>>(); \
  }

WABI_NAMESPACE_END

#endif  // WABI_BASE_PLUG_INTERFACE_FACTORY_H
