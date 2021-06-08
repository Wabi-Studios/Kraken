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

#ifndef WABI_USD_GDR_REGISTRY_H
#define WABI_USD_GDR_REGISTRY_H

/**
 * @file gdr/registry.h */

#include "wabi/base/tf/singleton.h"
#include "wabi/usd/gdr/api.h"
#include "wabi/usd/gdr/declare.h"
#include "wabi/usd/gdr/geomNode.h"
#include "wabi/usd/ndr/registry.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

/** @class GdrRegistry
 *
 * The GeomNode-specialized version of `NdrRegistry`. */
class GdrRegistry : public NdrRegistry {
 public:
  /**
   * Get the single `GdrRegistry` instance. */
  GDR_API
  static GdrRegistry &GetInstance();

  /**
   * Exactly like `NdrRegistry::GetNodeByIdentifier()`, but returns a
   * `GdrGeomNode` pointer instead of a `NdrNode` pointer. */
  GDR_API
  GdrGeomNodeConstPtr GetGeomNodeByIdentifier(const NdrIdentifier &identifier,
                                              const NdrTokenVec &typePriority = NdrTokenVec());

  /**
   * Exactly like `NdrRegistry::GetNodeByIdentifierAndType()`, but returns
   * a `GdrGeomNode` pointer instead of a `NdrNode` pointer. */
  GDR_API
  GdrGeomNodeConstPtr GetGeomNodeByIdentifierAndType(const NdrIdentifier &identifier,
                                                     const TfToken &nodeType);

  /**
   * Exactly like `NdrRegistry::GetNodeByName()`, but returns a
   * `GdrGeomNode` pointer instead of a `NdrNode` pointer. */
  GDR_API
  GdrGeomNodeConstPtr GetGeomNodeByName(const std::string &name,
                                        const NdrTokenVec &typePriority = NdrTokenVec(),
                                        NdrVersionFilter filter = NdrVersionFilterDefaultOnly);

  /**
   * Exactly like `NdrRegistry::GetNodeByNameAndType()`, but returns a
   * `GdrGeomNode` pointer instead of a `NdrNode` pointer. */
  GDR_API
  GdrGeomNodeConstPtr GetGeomNodeByNameAndType(
      const std::string &name,
      const TfToken &nodeType,
      NdrVersionFilter filter = NdrVersionFilterDefaultOnly);

  /**
   * Wrapper method for NdrRegistry::GetNodeFromAsset().
   * Returns a valid GdrGeomNode pointer upon success. */
  GDR_API
  GdrGeomNodeConstPtr GetGeomNodeFromAsset(const SdfAssetPath &geomAsset,
                                           const NdrTokenMap &metadata  = NdrTokenMap(),
                                           const TfToken &subIdentifier = TfToken(),
                                           const TfToken &sourceType    = TfToken());

  /**
   * Wrapper method for NdrRegistry::GetNodeFromSourceCode().
   * Returns a valid GdrGeomNode pointer upon success. */
  GDR_API
  GdrGeomNodeConstPtr GetGeomNodeFromSourceCode(const std::string &sourceCode,
                                                const TfToken &sourceType,
                                                const NdrTokenMap &metadata = NdrTokenMap());

  /**
   * Exactly like `NdrRegistry::GetNodesByIdentifier()`, but returns a vector
   * of `GdrGeomNode` pointers instead of a vector of `NdrNode` pointers. */
  GDR_API
  GdrGeomNodePtrVec GetGeomNodesByIdentifier(const NdrIdentifier &identifier);

  /**
   * Exactly like `NdrRegistry::GetNodesByName()`, but returns a vector of
   * `GdrGeomNode` pointers instead of a vector of `NdrNode` pointers. */
  GDR_API
  GdrGeomNodePtrVec GetGeomNodesByName(const std::string &name,
                                       NdrVersionFilter filter = NdrVersionFilterDefaultOnly);

  /**
   * Exactly like `NdrRegistry::GetNodesByFamily()`, but returns a vector of
   * `GdrGeomNode` pointers instead of a vector of `NdrNode` pointers. */
  GDR_API
  GdrGeomNodePtrVec GetGeomNodesByFamily(const TfToken &family   = TfToken(),
                                         NdrVersionFilter filter = NdrVersionFilterDefaultOnly);

 protected:
  /**
   * Allow TF to construct the class. */
  friend class TfSingleton<GdrRegistry>;

  GdrRegistry();
  ~GdrRegistry();
};

WABI_NAMESPACE_END

#endif /* WABI_USD_GDR_REGISTRY_H */
