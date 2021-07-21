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
#ifndef USDRI_GENERATED_TEXTUREAPI_H
#define USDRI_GENERATED_TEXTUREAPI_H

/// \file usdRi/textureAPI.h

#include "wabi/usd/usd/apiSchemaBase.h"
#include "wabi/usd/usd/prim.h"
#include "wabi/usd/usd/stage.h"
#include "wabi/usd/usdRi/api.h"
#include "wabi/usd/usdRi/tokens.h"
#include "wabi/wabi.h"

#include "wabi/base/vt/value.h"

#include "wabi/base/gf/matrix4d.h"
#include "wabi/base/gf/vec3d.h"
#include "wabi/base/gf/vec3f.h"

#include "wabi/base/tf/token.h"
#include "wabi/base/tf/type.h"

WABI_NAMESPACE_BEGIN

class SdfAssetPath;

// -------------------------------------------------------------------------- //
// RITEXTUREAPI                                                               //
// -------------------------------------------------------------------------- //

/// \class UsdRiTextureAPI
///
///
/// \deprecated This API schema will be removed in a future release.
///
/// RiTextureAPI is an API schema that provides an interface
/// to add Renderman-specific attributes to adjust textures.
///
class UsdRiTextureAPI : public UsdAPISchemaBase
{
 public:
  /// Compile time constant representing what kind of schema this class is.
  ///
  /// \sa UsdSchemaKind
  static const UsdSchemaKind schemaKind = UsdSchemaKind::SingleApplyAPI;


  /// Construct a UsdRiTextureAPI on UsdPrim \p prim .
  /// Equivalent to UsdRiTextureAPI::Get(prim.GetStage(), prim.GetPath())
  /// for a \em valid \p prim, but will not immediately throw an error for
  /// an invalid \p prim
  explicit UsdRiTextureAPI(const UsdPrim &prim = UsdPrim())
    : UsdAPISchemaBase(prim)
  {}

  /// Construct a UsdRiTextureAPI on the prim held by \p schemaObj .
  /// Should be preferred over UsdRiTextureAPI(schemaObj.GetPrim()),
  /// as it preserves SchemaBase state.
  explicit UsdRiTextureAPI(const UsdSchemaBase &schemaObj)
    : UsdAPISchemaBase(schemaObj)
  {}

  /// Destructor.
  USDRI_API
  virtual ~UsdRiTextureAPI();

  /// Return a vector of names of all pre-declared attributes for this schema
  /// class and all its ancestor classes.  Does not include attributes that
  /// may be authored by custom/extended methods of the schemas involved.
  USDRI_API
  static const TfTokenVector &GetSchemaAttributeNames(bool includeInherited = true);

  /// Return a UsdRiTextureAPI holding the prim adhering to this
  /// schema at \p path on \p stage.  If no prim exists at \p path on
  /// \p stage, or if the prim at that path does not adhere to this schema,
  /// return an invalid schema object.  This is shorthand for the following:
  ///
  /// \code
  /// UsdRiTextureAPI(stage->GetPrimAtPath(path));
  /// \endcode
  ///
  USDRI_API
  static UsdRiTextureAPI Get(const UsdStagePtr &stage, const SdfPath &path);

  /// Applies this <b>single-apply</b> API schema to the given \p prim.
  /// This information is stored by adding "RiTextureAPI" to the
  /// token-valued, listOp metadata \em apiSchemas on the prim.
  ///
  /// \return A valid UsdRiTextureAPI object is returned upon success.
  /// An invalid (or empty) UsdRiTextureAPI object is returned upon
  /// failure. See \ref UsdPrim::ApplyAPI() for conditions
  /// resulting in failure.
  ///
  /// \sa UsdPrim::GetAppliedSchemas()
  /// \sa UsdPrim::HasAPI()
  /// \sa UsdPrim::ApplyAPI()
  /// \sa UsdPrim::RemoveAPI()
  ///
  USDRI_API
  static UsdRiTextureAPI Apply(const UsdPrim &prim);

 protected:
  /// Returns the kind of schema this class belongs to.
  ///
  /// \sa UsdSchemaKind
  USDRI_API
  UsdSchemaKind GetSchemaKind() const override;

 private:
  // needs to invoke GetStaticTfType.
  friend class UsdSchemaRegistry;
  USDRI_API
  static const TfType &GetStaticTfType();

  static bool IsTypedSchema();

  // override SchemaBase virtuals.
  USDRI_API
  const TfType &GetTfType() const override;

 public:
  // --------------------------------------------------------------------- //
  // RITEXTUREGAMMA
  // --------------------------------------------------------------------- //
  /// Gamma-correct the texture
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `float ri:texture:gamma` |
  /// | C++ Type | float |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
  USDRI_API
  UsdAttribute GetRiTextureGammaAttr() const;

  /// See GetRiTextureGammaAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDRI_API
  UsdAttribute CreateRiTextureGammaAttr(VtValue const &defaultValue = VtValue(),
                                        bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // RITEXTURESATURATION
  // --------------------------------------------------------------------- //
  /// Adjust the texture's saturation
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `float ri:texture:saturation` |
  /// | C++ Type | float |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
  USDRI_API
  UsdAttribute GetRiTextureSaturationAttr() const;

  /// See GetRiTextureSaturationAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDRI_API
  UsdAttribute CreateRiTextureSaturationAttr(VtValue const &defaultValue = VtValue(),
                                             bool writeSparsely = false) const;

 public:
  // ===================================================================== //
  // Feel free to add custom code below this line, it will be preserved by
  // the code generator.
  //
  // Just remember to:
  //  - Close the class declaration with };
  //  - Close the namespace with WABI_NAMESPACE_END
  //  - Close the include guard with #endif
  // ===================================================================== //
  // --(BEGIN CUSTOM CODE)--
};

WABI_NAMESPACE_END

#endif
