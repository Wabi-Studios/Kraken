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
#ifndef USDRI_GENERATED_LIGHTFILTERAPI_H
#define USDRI_GENERATED_LIGHTFILTERAPI_H

/// \file usdRi/lightFilterAPI.h

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
// RILIGHTFILTERAPI                                                           //
// -------------------------------------------------------------------------- //

/// \class UsdRiLightFilterAPI
///
///
/// \deprecated RenderMan-specific light filter settings will move to a
/// new schema in a future release.
///
/// Renderman-specific attributes for light filters.
///
/// For any described attribute \em Fallback \em Value or \em Allowed \em Values below
/// that are text/tokens, the actual token is published and defined in \ref UsdRiTokens.
/// So to set an attribute to the value "rightHanded", use UsdRiTokens->rightHanded
/// as the value.
///
class UsdRiLightFilterAPI : public UsdAPISchemaBase
{
 public:
  /// Compile time constant representing what kind of schema this class is.
  ///
  /// \sa UsdSchemaKind
  static const UsdSchemaKind schemaKind = UsdSchemaKind::SingleApplyAPI;

  /// \deprecated
  /// Same as schemaKind, provided to maintain temporary backward
  /// compatibility with older generated schemas.
  static const UsdSchemaKind schemaType = UsdSchemaKind::SingleApplyAPI;

  /// Construct a UsdRiLightFilterAPI on UsdPrim \p prim .
  /// Equivalent to UsdRiLightFilterAPI::Get(prim.GetStage(), prim.GetPath())
  /// for a \em valid \p prim, but will not immediately throw an error for
  /// an invalid \p prim
  explicit UsdRiLightFilterAPI(const UsdPrim &prim = UsdPrim())
    : UsdAPISchemaBase(prim)
  {}

  /// Construct a UsdRiLightFilterAPI on the prim held by \p schemaObj .
  /// Should be preferred over UsdRiLightFilterAPI(schemaObj.GetPrim()),
  /// as it preserves SchemaBase state.
  explicit UsdRiLightFilterAPI(const UsdSchemaBase &schemaObj)
    : UsdAPISchemaBase(schemaObj)
  {}

  /// Destructor.
  USDRI_API
  virtual ~UsdRiLightFilterAPI();

  /// Return a vector of names of all pre-declared attributes for this schema
  /// class and all its ancestor classes.  Does not include attributes that
  /// may be authored by custom/extended methods of the schemas involved.
  USDRI_API
  static const TfTokenVector &GetSchemaAttributeNames(bool includeInherited = true);

  /// Return a UsdRiLightFilterAPI holding the prim adhering to this
  /// schema at \p path on \p stage.  If no prim exists at \p path on
  /// \p stage, or if the prim at that path does not adhere to this schema,
  /// return an invalid schema object.  This is shorthand for the following:
  ///
  /// \code
  /// UsdRiLightFilterAPI(stage->GetPrimAtPath(path));
  /// \endcode
  ///
  USDRI_API
  static UsdRiLightFilterAPI Get(const UsdStagePtr &stage, const SdfPath &path);

  /// Applies this <b>single-apply</b> API schema to the given \p prim.
  /// This information is stored by adding "RiLightFilterAPI" to the
  /// token-valued, listOp metadata \em apiSchemas on the prim.
  ///
  /// \return A valid UsdRiLightFilterAPI object is returned upon success.
  /// An invalid (or empty) UsdRiLightFilterAPI object is returned upon
  /// failure. See \ref UsdPrim::ApplyAPI() for conditions
  /// resulting in failure.
  ///
  /// \sa UsdPrim::GetAppliedSchemas()
  /// \sa UsdPrim::HasAPI()
  /// \sa UsdPrim::ApplyAPI()
  /// \sa UsdPrim::RemoveAPI()
  ///
  USDRI_API
  static UsdRiLightFilterAPI Apply(const UsdPrim &prim);

 protected:
  /// Returns the kind of schema this class belongs to.
  ///
  /// \sa UsdSchemaKind
  USDRI_API
  UsdSchemaKind _GetSchemaKind() const override;

  /// \deprecated
  /// Same as _GetSchemaKind, provided to maintain temporary backward
  /// compatibility with older generated schemas.
  USDRI_API
  UsdSchemaKind _GetSchemaType() const override;

 private:
  // needs to invoke _GetStaticTfType.
  friend class UsdSchemaRegistry;
  USDRI_API
  static const TfType &_GetStaticTfType();

  static bool _IsTypedSchema();

  // override SchemaBase virtuals.
  USDRI_API
  const TfType &_GetTfType() const override;

 public:
  // --------------------------------------------------------------------- //
  // RICOMBINEMODE
  // --------------------------------------------------------------------- //
  /// Specifies how this filter combines with others.
  /// Valid values are:
  ///
  /// - multiply: The results of filters are multiplied together
  /// - max: The maximum result of the filters is used.  This
  /// works best for grey-scale filters.
  /// - min: The minimum result of the filters is used. This
  /// works best for grey-scale filters.
  /// - screen: Similar to max, but combines gradients in a smoother
  /// way by using a screen operation:
  /// <pre>screen(a,b) = 1-(1-a)(1-b)</pre>
  /// This works best for grey-scale filters.
  ///
  /// Light filters on a light are grouped by their combine mode.
  /// Each group is executed and combined using that mode.  Then,
  /// the final results of each group are multiplied together.
  ///
  /// Fallback: multiply
  ///
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `token ri:combineMode = "multiply"` |
  /// | C++ Type | TfToken |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Token |
  /// | \ref UsdRiTokens "Allowed Values" | multiply, max, min, screen |
  USDRI_API
  UsdAttribute GetRiCombineModeAttr() const;

  /// See GetRiCombineModeAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDRI_API
  UsdAttribute CreateRiCombineModeAttr(VtValue const &defaultValue = VtValue(),
                                       bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // RIDENSITY
  // --------------------------------------------------------------------- //
  /// Scales the strength of the filter.
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `float ri:density = 1` |
  /// | C++ Type | float |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
  USDRI_API
  UsdAttribute GetRiDensityAttr() const;

  /// See GetRiDensityAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDRI_API
  UsdAttribute CreateRiDensityAttr(VtValue const &defaultValue = VtValue(),
                                   bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // RIINVERT
  // --------------------------------------------------------------------- //
  /// When true, inverts the output of the light filter.
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `bool ri:invert = 0` |
  /// | C++ Type | bool |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Bool |
  USDRI_API
  UsdAttribute GetRiInvertAttr() const;

  /// See GetRiInvertAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDRI_API
  UsdAttribute CreateRiInvertAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // RIINTENSITY
  // --------------------------------------------------------------------- //
  /// Multipier for the diffuse and specular result.
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `float ri:intensity = 0` |
  /// | C++ Type | float |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
  USDRI_API
  UsdAttribute GetRiIntensityAttr() const;

  /// See GetRiIntensityAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDRI_API
  UsdAttribute CreateRiIntensityAttr(VtValue const &defaultValue = VtValue(),
                                     bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // RIEXPOSURE
  // --------------------------------------------------------------------- //
  /// Exposure control for the multiplier.
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `float ri:exposure = 0` |
  /// | C++ Type | float |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
  USDRI_API
  UsdAttribute GetRiExposureAttr() const;

  /// See GetRiExposureAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDRI_API
  UsdAttribute CreateRiExposureAttr(VtValue const &defaultValue = VtValue(),
                                    bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // RIDIFFUSE
  // --------------------------------------------------------------------- //
  /// A multiplier for the effect of this light on the diffuse
  /// response of materials.  This is a non-physical control.
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `float ri:diffuse = 1` |
  /// | C++ Type | float |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
  USDRI_API
  UsdAttribute GetRiDiffuseAttr() const;

  /// See GetRiDiffuseAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDRI_API
  UsdAttribute CreateRiDiffuseAttr(VtValue const &defaultValue = VtValue(),
                                   bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // RISPECULAR
  // --------------------------------------------------------------------- //
  /// A multiplier for the effect of this light on the specular
  /// response of materials.  This is a non-physical control.
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `float ri:specular = 1` |
  /// | C++ Type | float |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
  USDRI_API
  UsdAttribute GetRiSpecularAttr() const;

  /// See GetRiSpecularAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDRI_API
  UsdAttribute CreateRiSpecularAttr(VtValue const &defaultValue = VtValue(),
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
