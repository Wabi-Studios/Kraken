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
#ifndef WABI_USD_SDF_VALUE_TYPE_PRIVATE_H
#define WABI_USD_SDF_VALUE_TYPE_PRIVATE_H

#include "wabi/base/tf/enum.h"
#include "wabi/base/tf/type.h"
#include "wabi/base/vt/value.h"
#include "wabi/usd/sdf/valueTypeName.h"
#include "wabi/wabi.h"
#include <vector>

WABI_NAMESPACE_BEGIN

class Sdf_ValueTypeImpl;

struct Sdf_ValueTypePrivate
{
 public:
  struct Empty
  {
  };

  // Represents a type/role pair.
  struct CoreType
  {
    CoreType()
    {}
    CoreType(Empty);

    TfType type;
    std::string cppTypeName;
    TfToken role;
    SdfTupleDimensions dim;
    VtValue value;
    TfEnum unit;

    // All type names aliasing this type/role pair in registration order.
    // The first alias is the "fundamental" type name.
    std::vector<TfToken> aliases;
  };

  /// Construct a SdfValueTypeName.
  static SdfValueTypeName MakeValueTypeName(const Sdf_ValueTypeImpl *impl);

  /// Return the value type implementation representing the empty type name.
  static const Sdf_ValueTypeImpl *GetEmptyTypeName();
};

/// Represents a registered type name.
class Sdf_ValueTypeImpl
{
 public:
  Sdf_ValueTypeImpl();

  const Sdf_ValueTypePrivate::CoreType *type;
  TfToken name;
  const Sdf_ValueTypeImpl *scalar;
  const Sdf_ValueTypeImpl *array;
};

WABI_NAMESPACE_END

#endif  // WABI_USD_SDF_VALUE_TYPE_PRIVATE_H
