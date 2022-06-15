//
// Copyright 2021 Pixar
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
////////////////////////////////////////////////////////////////////////
// This file is generated by a script.  Do not edit directly.  Edit the
// schema.template.h file to make changes.

#ifndef WABI_IMAGING_HD_MESH_TOPOLOGY_SCHEMA_H
#define WABI_IMAGING_HD_MESH_TOPOLOGY_SCHEMA_H

#include "wabi/imaging/hd/api.h"

#include "wabi/imaging/hd/subdivisionTagsSchema.h"


WABI_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------

#define HDMESHTOPOLOGY_SCHEMA_TOKENS \
  (topology)(faceVertexCounts)(      \
    faceVertexIndices)(holeIndices)(orientation)(leftHanded)(rightHanded)

TF_DECLARE_PUBLIC_TOKENS(HdMeshTopologySchemaTokens, HD_API, HDMESHTOPOLOGY_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------

class HdMeshTopologySchema : public HdSchema
{
 public:

  HdMeshTopologySchema(HdContainerDataSourceHandle container) : HdSchema(container) {}

  // ACCESSORS

  HD_API
  HdIntArrayDataSourceHandle GetFaceVertexCounts();
  HD_API
  HdIntArrayDataSourceHandle GetFaceVertexIndices();
  HD_API
  HdIntArrayDataSourceHandle GetHoleIndices();
  HD_API
  HdTokenDataSourceHandle GetOrientation();

  // RETRIEVING AND CONSTRUCTING

  /// Builds a container data source which includes the provided child data
  /// sources. Parameters with nullptr values are excluded. This is a
  /// low-level interface. For cases in which it's desired to define
  /// the container with a sparse set of child fields, the Builder class
  /// is often more convenient and readable.
  HD_API
  static HdContainerDataSourceHandle BuildRetained(
    const HdIntArrayDataSourceHandle &faceVertexCounts,
    const HdIntArrayDataSourceHandle &faceVertexIndices,
    const HdIntArrayDataSourceHandle &holeIndices,
    const HdTokenDataSourceHandle &orientation);

  /// \class HdMeshTopologySchema::Builder
  ///
  /// Utility class for setting sparse sets of child data source fields to be
  /// filled as arguments into BuildRetained. Because all setter methods
  /// return a reference to the instance, this can be used in the "builder
  /// pattern" form.
  class Builder
  {
   public:

    HD_API
    Builder &SetFaceVertexCounts(const HdIntArrayDataSourceHandle &faceVertexCounts);
    HD_API
    Builder &SetFaceVertexIndices(const HdIntArrayDataSourceHandle &faceVertexIndices);
    HD_API
    Builder &SetHoleIndices(const HdIntArrayDataSourceHandle &holeIndices);
    HD_API
    Builder &SetOrientation(const HdTokenDataSourceHandle &orientation);

    /// Returns a container data source containing the members set thus far.
    HD_API
    HdContainerDataSourceHandle Build();

   private:

    HdIntArrayDataSourceHandle _faceVertexCounts;
    HdIntArrayDataSourceHandle _faceVertexIndices;
    HdIntArrayDataSourceHandle _holeIndices;
    HdTokenDataSourceHandle _orientation;
  };

  /// Retrieves a container data source with the schema's default name token
  /// "topology" from the parent container and constructs a
  /// HdMeshTopologySchema instance.
  /// Because the requested container data source may not exist, the result
  /// should be checked with IsDefined() or a bool comparison before use.
  HD_API
  static HdMeshTopologySchema GetFromParent(
    const HdContainerDataSourceHandle &fromParentContainer);

  /// Returns an HdDataSourceLocator (relative to the prim-level data source)
  /// where the container representing this schema is found by default.
  HD_API
  static const HdDataSourceLocator &GetDefaultLocator();

  /// Returns token data source for use as orientation value.
  /// Values of...
  /// - HdMeshTopologySchemaTokens->leftHanded
  /// - HdMeshTopologySchemaTokens->rightHanded
  ///     ...will be stored statically and reused for future calls.
  HD_API
  static HdTokenDataSourceHandle BuildOrientationDataSource(const TfToken &orientation);
};

WABI_NAMESPACE_END

#endif