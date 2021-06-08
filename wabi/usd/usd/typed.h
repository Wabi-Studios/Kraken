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
#ifndef WABI_USD_USD_TYPED_H
#define WABI_USD_USD_TYPED_H

#include "wabi/usd/usd/api.h"
#include "wabi/usd/usd/prim.h"
#include "wabi/usd/usd/schemaBase.h"
#include "wabi/usd/usd/stage.h"
#include "wabi/wabi.h"

#include "wabi/base/tf/token.h"

WABI_NAMESPACE_BEGIN

/// \class UsdTyped
///
/// The base class for all \em typed schemas (those that can impart a
/// typeName to a UsdPrim), and therefore the base class for all
/// instantiable and "IsA" schemas.
///
/// UsdTyped implements a typeName-based query for its override of
/// UsdSchemaBase::_IsCompatible().  It provides no other behavior.
///
class UsdTyped : public UsdSchemaBase {
 public:
  /// Compile time constant representing what kind of schema this class is.
  ///
  /// \sa UsdSchemaKind
  static const UsdSchemaKind schemaKind = UsdSchemaKind::AbstractBase;

  /// \deprecated
  /// Same as schemaKind, provided to maintain temporary backward
  /// compatibility with older generated schemas.
  static const UsdSchemaKind schemaType = UsdSchemaKind::AbstractBase;

  /// Construct a UsdTyped on UsdPrim \p prim .
  /// Equivalent to UsdTyped::Get(prim.GetStage(), prim.GetPath())
  /// for a \em valid \p prim, but will not immediately throw an error for
  /// an invalid \p prim
  explicit UsdTyped(const UsdPrim &prim = UsdPrim()) : UsdSchemaBase(prim)
  {}

  /// Construct a UsdTyped on the prim wrapped by \p schemaObj .
  /// Should be preferred over UsdTyped(schemaObj.GetPrim()),
  /// as it preserves SchemaBase state.
  explicit UsdTyped(const UsdSchemaBase &schemaObj) : UsdSchemaBase(schemaObj)
  {}

  USD_API
  virtual ~UsdTyped();

  /// Return a vector of names of all pre-declared attributes for this schema
  /// class and all its ancestor classes.  Does not include attributes that
  /// may be authored by custom/extended methods of the schemas involved.
  static const TfTokenVector &GetSchemaAttributeNames(bool includeInherited = true)
  {
    /* This only exists for consistency */
    static TfTokenVector names;
    return names;
  }

  /// Return a UsdTyped holding the prim adhering to this schema at \p path
  /// on \p stage.  If no prim exists at \p path on \p stage, or if the prim
  /// at that path does not adhere to this schema, return an invalid schema
  /// object.  This is shorthand for the following:
  ///
  /// \code
  /// UsdTyped(stage->GetPrimAtPath(path));
  /// \endcode
  ///
  USD_API
  static UsdTyped Get(const UsdStagePtr &stage, const SdfPath &path);

 protected:
  USD_API
  bool _IsCompatible() const override;

 private:
  USD_API
  const TfType &_GetTfType() const override;
};

WABI_NAMESPACE_END

#endif  // WABI_USD_USD_TYPED_H
