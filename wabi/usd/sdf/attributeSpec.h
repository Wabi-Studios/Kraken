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
#ifndef WABI_USD_SDF_ATTRIBUTE_SPEC_H
#define WABI_USD_SDF_ATTRIBUTE_SPEC_H

/// \file sdf/attributeSpec.h

#include "wabi/base/tf/enum.h"
#include "wabi/usd/sdf/api.h"
#include "wabi/usd/sdf/declareSpec.h"
#include "wabi/usd/sdf/path.h"
#include "wabi/usd/sdf/propertySpec.h"
#include "wabi/usd/sdf/types.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

/// \class SdfAttributeSpec
///
/// A subclass of SdfPropertySpec that holds typed data.
///
/// Attributes are typed data containers that can optionally hold any
/// and all of the following:
/// \li A single default value.
/// \li An array of knot values describing how the value varies over time.
/// \li A dictionary of posed values, indexed by name.
///
/// The values contained in an attribute must all be of the same type.  In the
/// Python API the \c typeName property holds the attribute type.  In the C++
/// API, you can get the attribute type using the GetTypeName() method.  In
/// addition, all values, including all knot values, must be the same shape.
/// For information on shapes, see the VtShape class reference in the C++
/// documentation.
///
class SdfAttributeSpec : public SdfPropertySpec {
  SDF_DECLARE_SPEC(SdfAttributeSpec, SdfPropertySpec);

 public:
  typedef SdfAttributeSpec This;
  typedef SdfPropertySpec Parent;

  ///
  /// \name Spec construction
  /// @{

  /// Constructs a new prim attribute instance.
  ///
  /// Creates and returns a new attribute for the given prim.
  /// The \p owner will own the newly created attribute.
  SDF_API
  static SdfAttributeSpecHandle New(const SdfPrimSpecHandle &owner,
                                    const std::string &name,
                                    const SdfValueTypeName &typeName,
                                    SdfVariability variability = SdfVariabilityVarying,
                                    bool custom                = false);

  /// @}

  /// \name Connections
  /// @{

  /// Returns a proxy for editing the attribute's connection paths.
  ///
  /// The returned proxy, which is an SdfListEditorProxy, modifies the
  /// SdfListOp that represents this attribute's connections.
  SDF_API
  SdfConnectionsProxy GetConnectionPathList() const;

  /// Returns \c true if any connection paths are set on this attribute.
  SDF_API
  bool HasConnectionPaths() const;

  /// Clears the connection paths for this attribute.
  SDF_API
  void ClearConnectionPaths();

  /// @}
  /// \name Attribute value API
  /// @{

  /// Returns the allowed tokens metadata for this attribute.
  /// Consumers may use this metadata to define a set of predefined
  /// options for this attribute's value. However, this metadata is
  /// purely advisory. It is up to the consumer to perform any
  /// validation against this set of tokens, if desired.
  SDF_API
  VtTokenArray GetAllowedTokens() const;

  /// Sets the allowed tokens metadata for this attribute.
  SDF_API
  void SetAllowedTokens(const VtTokenArray &allowedTokens);

  /// Returns true if allowed tokens metadata is set for this attribute.
  SDF_API
  bool HasAllowedTokens() const;

  /// Clears the allowed tokens metadata for this attribute.
  SDF_API
  void ClearAllowedTokens();

  /// Returns the display unit of the attribute.
  SDF_API
  TfEnum GetDisplayUnit() const;

  /// Sets the display unit of the attribute.
  SDF_API
  void SetDisplayUnit(const TfEnum &displayUnit);

  /// Returns true if a display unit is set for this attribute.
  SDF_API
  bool HasDisplayUnit() const;

  /// Clears the display unit of the attribute.
  SDF_API
  void ClearDisplayUnit();

  /// Returns the color-space in which a color or texture valued attribute
  /// is authored.
  SDF_API
  TfToken GetColorSpace() const;

  /// Sets the color-space in which a color or texture valued attribute is
  /// authored.
  SDF_API
  void SetColorSpace(const TfToken &colorSpace);

  /// Returns true if this attribute has a colorSpace value authored.
  SDF_API
  bool HasColorSpace() const;

  /// Clears the colorSpace metadata value set on this attribute.
  SDF_API
  void ClearColorSpace();

  /// @}
  /// \name Spec properties
  /// @{

  /// Returns the roleName for this attribute's typeName.
  ///
  /// If the typeName has no roleName, return empty token.
  SDF_API
  TfToken GetRoleName() const;

  /// @}
};

/// Convenience function to create an attributeSpec on a primSpec at the given
/// path, and any necessary parent primSpecs, in the given layer.
///
/// If an attributeSpec already exists at the given path, just author typeName,
/// variability, and custom according to passed arguments and return true.
///
/// Any newly created prim specs have SdfSpecifierOver and an empty type (as if
/// created by SdfJustCreatePrimInLayer()).  attrPath must be a valid prim
/// property path (see SdfPath::IsPrimPropertyPath()).  Return false and issue
/// an error if we fail to author the required scene description.
SDF_API
bool SdfJustCreatePrimAttributeInLayer(const SdfLayerHandle &layer,
                                       const SdfPath &attrPath,
                                       const SdfValueTypeName &typeName,
                                       SdfVariability variability = SdfVariabilityVarying,
                                       bool isCustom              = false);

WABI_NAMESPACE_END

#endif  // WABI_USD_SDF_ATTRIBUTE_SPEC_H
