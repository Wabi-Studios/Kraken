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
#include "wabi/imaging/hdPh/materialParam.h"

#include "wabi/base/tf/staticTokens.h"

#include <boost/functional/hash.hpp>

WABI_NAMESPACE_BEGIN

HdPh_MaterialParam::HdPh_MaterialParam()
    : paramType(ParamTypeFallback),
      name(),
      fallbackValue(),
      samplerCoords(),
      textureType(HdTextureType::Uv),
      swizzle(),
      isPremultiplied(false)
{}

HdPh_MaterialParam::HdPh_MaterialParam(ParamType paramType,
                                       TfToken const &name,
                                       VtValue const &fallbackValue,
                                       TfTokenVector const &samplerCoords,
                                       HdTextureType textureType,
                                       std::string const &swizzle,
                                       bool const isPremultiplied)
    : paramType(paramType),
      name(name),
      fallbackValue(fallbackValue),
      samplerCoords(samplerCoords),
      textureType(textureType),
      swizzle(swizzle),
      isPremultiplied(isPremultiplied)
{}

size_t HdPh_MaterialParam::ComputeHash(HdPh_MaterialParamVector const &params)
{
  size_t hash = 0;
  for (HdPh_MaterialParam const &param : params) {
    boost::hash_combine(hash, param.paramType);
    boost::hash_combine(hash, param.name.Hash());
    for (TfToken const &coord : param.samplerCoords) {
      boost::hash_combine(hash, coord.Hash());
    }
    boost::hash_combine(hash, param.textureType);
    boost::hash_combine(hash, param.swizzle);
    boost::hash_combine(hash, param.isPremultiplied);
  }
  return hash;
}

HdTupleType HdPh_MaterialParam::GetTupleType() const
{
  return HdGetValueTupleType(fallbackValue);
}

WABI_NAMESPACE_END
