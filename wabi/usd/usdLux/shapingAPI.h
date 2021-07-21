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
#ifndef USDLUX_GENERATED_SHAPINGAPI_H
#define USDLUX_GENERATED_SHAPINGAPI_H

/// \file usdLux/shapingAPI.h

#include "wabi/usd/usd/apiSchemaBase.h"
#include "wabi/usd/usd/prim.h"
#include "wabi/usd/usd/stage.h"
#include "wabi/usd/usdLux/api.h"
#include "wabi/usd/usdLux/tokens.h"
#include "wabi/wabi.h"

#include "wabi/usd/usdShade/input.h"
#include "wabi/usd/usdShade/output.h"

#include "wabi/base/vt/value.h"

#include "wabi/base/gf/matrix4d.h"
#include "wabi/base/gf/vec3d.h"
#include "wabi/base/gf/vec3f.h"

#include "wabi/base/tf/token.h"
#include "wabi/base/tf/type.h"

WABI_NAMESPACE_BEGIN

class SdfAssetPath;

// -------------------------------------------------------------------------- //
// SHAPINGAPI                                                                 //
// -------------------------------------------------------------------------- //

/// \class UsdLuxShapingAPI
///
/// Controls for shaping a light's emission.
///
class UsdLuxShapingAPI : public UsdAPISchemaBase
{
 public:
  /// Compile time constant representing what kind of schema this class is.
  ///
  /// \sa UsdSchemaKind
  static const UsdSchemaKind schemaKind = UsdSchemaKind::SingleApplyAPI;



  /// Construct a UsdLuxShapingAPI on UsdPrim \p prim .
  /// Equivalent to UsdLuxShapingAPI::Get(prim.GetStage(), prim.GetPath())
  /// for a \em valid \p prim, but will not immediately throw an error for
  /// an invalid \p prim
  explicit UsdLuxShapingAPI(const UsdPrim &prim = UsdPrim())
    : UsdAPISchemaBase(prim)
  {}

  /// Construct a UsdLuxShapingAPI on the prim held by \p schemaObj .
  /// Should be preferred over UsdLuxShapingAPI(schemaObj.GetPrim()),
  /// as it preserves SchemaBase state.
  explicit UsdLuxShapingAPI(const UsdSchemaBase &schemaObj)
    : UsdAPISchemaBase(schemaObj)
  {}

  /// Destructor.
  USDLUX_API
  virtual ~UsdLuxShapingAPI();

  /// Return a vector of names of all pre-declared attributes for this schema
  /// class and all its ancestor classes.  Does not include attributes that
  /// may be authored by custom/extended methods of the schemas involved.
  USDLUX_API
  static const TfTokenVector &GetSchemaAttributeNames(bool includeInherited = true);

  /// Return a UsdLuxShapingAPI holding the prim adhering to this
  /// schema at \p path on \p stage.  If no prim exists at \p path on
  /// \p stage, or if the prim at that path does not adhere to this schema,
  /// return an invalid schema object.  This is shorthand for the following:
  ///
  /// \code
  /// UsdLuxShapingAPI(stage->GetPrimAtPath(path));
  /// \endcode
  ///
  USDLUX_API
  static UsdLuxShapingAPI Get(const UsdStagePtr &stage, const SdfPath &path);

  /// Applies this <b>single-apply</b> API schema to the given \p prim.
  /// This information is stored by adding "ShapingAPI" to the
  /// token-valued, listOp metadata \em apiSchemas on the prim.
  ///
  /// \return A valid UsdLuxShapingAPI object is returned upon success.
  /// An invalid (or empty) UsdLuxShapingAPI object is returned upon
  /// failure. See \ref UsdPrim::ApplyAPI() for conditions
  /// resulting in failure.
  ///
  /// \sa UsdPrim::GetAppliedSchemas()
  /// \sa UsdPrim::HasAPI()
  /// \sa UsdPrim::ApplyAPI()
  /// \sa UsdPrim::RemoveAPI()
  ///
  USDLUX_API
  static UsdLuxShapingAPI Apply(const UsdPrim &prim);

 protected:
  /// Returns the kind of schema this class belongs to.
  ///
  /// \sa UsdSchemaKind
  USDLUX_API
  UsdSchemaKind GetSchemaKind() const override;

 private:
  // needs to invoke GetStaticTfType.
  friend class UsdSchemaRegistry;
  USDLUX_API
  static const TfType &GetStaticTfType();

  static bool IsTypedSchema();

  // override SchemaBase virtuals.
  USDLUX_API
  const TfType &GetTfType() const override;

 public:
  // --------------------------------------------------------------------- //
  // SHAPING:FOCUS
  // --------------------------------------------------------------------- //
  /// A control to shape the spread of light.  Higher focus
  /// values pull light towards the center and narrow the spread.
  /// Implemented as an off-axis cosine power exponent.
  /// TODO: clarify semantics
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `float inputs:shaping:focus = 0` |
  /// | C++ Type | float |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
  USDLUX_API
  UsdAttribute GetShapingFocusAttr() const;

  /// See GetShapingFocusAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDLUX_API
  UsdAttribute CreateShapingFocusAttr(VtValue const &defaultValue = VtValue(),
                                      bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // SHAPING:FOCUSTINT
  // --------------------------------------------------------------------- //
  /// Off-axis color tint.  This tints the emission in the
  /// falloff region.  The default tint is black.
  /// TODO: clarify semantics
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `color3f inputs:shaping:focusTint = (0, 0, 0)` |
  /// | C++ Type | GfVec3f |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Color3f |
  USDLUX_API
  UsdAttribute GetShapingFocusTintAttr() const;

  /// See GetShapingFocusTintAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDLUX_API
  UsdAttribute CreateShapingFocusTintAttr(VtValue const &defaultValue = VtValue(),
                                          bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // SHAPING:CONE:ANGLE
  // --------------------------------------------------------------------- //
  /// Angular limit off the primary axis to restrict the
  /// light spread.
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `float inputs:shaping:cone:angle = 90` |
  /// | C++ Type | float |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
  USDLUX_API
  UsdAttribute GetShapingConeAngleAttr() const;

  /// See GetShapingConeAngleAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDLUX_API
  UsdAttribute CreateShapingConeAngleAttr(VtValue const &defaultValue = VtValue(),
                                          bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // SHAPING:CONE:SOFTNESS
  // --------------------------------------------------------------------- //
  /// Controls the cutoff softness for cone angle.
  /// TODO: clarify semantics
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `float inputs:shaping:cone:softness = 0` |
  /// | C++ Type | float |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
  USDLUX_API
  UsdAttribute GetShapingConeSoftnessAttr() const;

  /// See GetShapingConeSoftnessAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDLUX_API
  UsdAttribute CreateShapingConeSoftnessAttr(VtValue const &defaultValue = VtValue(),
                                             bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // SHAPING:IES:FILE
  // --------------------------------------------------------------------- //
  /// An IES (Illumination Engineering Society) light
  /// profile describing the angular distribution of light.
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `asset inputs:shaping:ies:file` |
  /// | C++ Type | SdfAssetPath |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Asset |
  USDLUX_API
  UsdAttribute GetShapingIesFileAttr() const;

  /// See GetShapingIesFileAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDLUX_API
  UsdAttribute CreateShapingIesFileAttr(VtValue const &defaultValue = VtValue(),
                                        bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // SHAPING:IES:ANGLESCALE
  // --------------------------------------------------------------------- //
  /// Rescales the angular distribution of the IES profile.
  /// TODO: clarify semantics
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `float inputs:shaping:ies:angleScale = 0` |
  /// | C++ Type | float |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
  USDLUX_API
  UsdAttribute GetShapingIesAngleScaleAttr() const;

  /// See GetShapingIesAngleScaleAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDLUX_API
  UsdAttribute CreateShapingIesAngleScaleAttr(VtValue const &defaultValue = VtValue(),
                                              bool writeSparsely = false) const;

 public:
  // --------------------------------------------------------------------- //
  // SHAPING:IES:NORMALIZE
  // --------------------------------------------------------------------- //
  /// Normalizes the IES profile so that it affects the shaping
  /// of the light while preserving the overall energy output.
  ///
  /// | ||
  /// | -- | -- |
  /// | Declaration | `bool inputs:shaping:ies:normalize = 0` |
  /// | C++ Type | bool |
  /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Bool |
  USDLUX_API
  UsdAttribute GetShapingIesNormalizeAttr() const;

  /// See GetShapingIesNormalizeAttr(), and also
  /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
  /// If specified, author \p defaultValue as the attribute's default,
  /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
  /// the default for \p writeSparsely is \c false.
  USDLUX_API
  UsdAttribute CreateShapingIesNormalizeAttr(VtValue const &defaultValue = VtValue(),
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

  // -------------------------------------------------------------------------
  /// \name Conversion to and from UsdShadeConnectableAPI
  ///
  /// @{

  /// Constructor that takes a ConnectableAPI object.
  /// Allow implicit conversion of UsdShadeConnectableAPI to
  /// UsdLuxShapingAPI.
  USDLUX_API
  UsdLuxShapingAPI(const UsdShadeConnectableAPI &connectable);

  /// Contructs and returns a UsdShadeConnectableAPI object with this shaping
  /// API prim. Note that a valid UsdLuxShapingAPI will only return a valid
  /// UsdShadeConnectableAPI if the its prim's Typed schema type is actually
  /// connectable.
  USDLUX_API
  UsdShadeConnectableAPI ConnectableAPI() const;

  /// @}

  // -------------------------------------------------------------------------
  /// \name Outputs API
  ///
  /// Outputs represent a typed attribute on a shaping API whose value is
  /// computed externally.
  ///
  /// @{

  /// Create an output which can either have a value or can be connected.
  /// The attribute representing the output is created in the "outputs:"
  /// namespace. Outputs on a shaping API cannot be connected, as their
  /// value is assumed to be computed externally.
  ///
  USDLUX_API
  UsdShadeOutput CreateOutput(const TfToken &name, const SdfValueTypeName &typeName);

  /// Return the requested output if it exists.
  ///
  USDLUX_API
  UsdShadeOutput GetOutput(const TfToken &name) const;

  /// Outputs are represented by attributes in the "outputs:" namespace.
  /// If \p onlyAuthored is true (the default), then only return authored
  /// attributes; otherwise, this also returns un-authored builtins.
  ///
  USDLUX_API
  std::vector<UsdShadeOutput> GetOutputs(bool onlyAuthored = true) const;

  /// @}

  // -------------------------------------------------------------------------

  /// \name Inputs API
  ///
  /// Inputs are connectable attribute with a typed value.
  ///
  /// Shaping API parameters are encoded as inputs.
  ///
  /// @{

  /// Create an input which can either have a value or can be connected.
  /// The attribute representing the input is created in the "inputs:"
  /// namespace. Inputs on shaping API are connectable.
  ///
  USDLUX_API
  UsdShadeInput CreateInput(const TfToken &name, const SdfValueTypeName &typeName);

  /// Return the requested input if it exists.
  ///
  USDLUX_API
  UsdShadeInput GetInput(const TfToken &name) const;

  /// Inputs are represented by attributes in the "inputs:" namespace.
  /// If \p onlyAuthored is true (the default), then only return authored
  /// attributes; otherwise, this also returns un-authored builtins.
  ///
  USDLUX_API
  std::vector<UsdShadeInput> GetInputs(bool onlyAuthored = true) const;

  /// @}
};

WABI_NAMESPACE_END

#endif
