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

#ifndef WABI_IMAGING_HD_VISIBILITY_SCHEMA_H
#define WABI_IMAGING_HD_VISIBILITY_SCHEMA_H

#include "wabi/imaging/hd/api.h"

#include "wabi/imaging/hd/schema.h"

WABI_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------

#define HDVISIBILITY_SCHEMA_TOKENS (visibility)


TF_DECLARE_PUBLIC_TOKENS(HdVisibilitySchemaTokens, HD_API, HDVISIBILITY_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------

class HdVisibilitySchema : public HdSchema
{
 public:

  HdVisibilitySchema(HdContainerDataSourceHandle container) : HdSchema(container) {}

  // ACCESSORS

  HD_API
  HdBoolDataSourceHandle GetVisibility();

  // RETRIEVING AND CONSTRUCTING

  /// Builds a container data source which includes the provided child data
  /// sources. Parameters with nullptr values are excluded. This is a
  /// low-level interface. For cases in which it's desired to define
  /// the container with a sparse set of child fields, the Builder class
  /// is often more convenient and readable.
  HD_API
  static HdContainerDataSourceHandle BuildRetained(const HdBoolDataSourceHandle &visibility);

  /// \class HdVisibilitySchema::Builder
  ///
  /// Utility class for setting sparse sets of child data source fields to be
  /// filled as arguments into BuildRetained. Because all setter methods
  /// return a reference to the instance, this can be used in the "builder
  /// pattern" form.
  class Builder
  {
   public:

    HD_API
    Builder &SetVisibility(const HdBoolDataSourceHandle &visibility);

    /// Returns a container data source containing the members set thus far.
    HD_API
    HdContainerDataSourceHandle Build();

   private:

    HdBoolDataSourceHandle _visibility;
  };

  /// Retrieves a container data source with the schema's default name token
  /// "visibility" from the parent container and constructs a
  /// HdVisibilitySchema instance.
  /// Because the requested container data source may not exist, the result
  /// should be checked with IsDefined() or a bool comparison before use.
  HD_API
  static HdVisibilitySchema GetFromParent(const HdContainerDataSourceHandle &fromParentContainer);

  /// Returns an HdDataSourceLocator (relative to the prim-level data source)
  /// where the container representing this schema is found by default.
  HD_API
  static const HdDataSourceLocator &GetDefaultLocator();
};

WABI_NAMESPACE_END

#endif