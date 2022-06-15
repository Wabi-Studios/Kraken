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
#ifndef WABI_IMAGING_GLF_BINDING_MAP_H
#define WABI_IMAGING_GLF_BINDING_MAP_H

/// \file glf/bindingMap.h

#include "wabi/base/tf/refBase.h"
#include "wabi/base/tf/stringUtils.h"
#include "wabi/base/tf/token.h"
#include "wabi/base/tf/weakBase.h"
#include "wabi/imaging/garch/glApi.h"
#include "wabi/imaging/glf/api.h"
#include "wabi/wabi.h"

#include "wabi/base/tf/hashmap.h"

WABI_NAMESPACE_BEGIN

class GlfBindingMap : public TfRefBase, public TfWeakBase
{
 public:

  typedef TfHashMap<TfToken, int, TfToken::HashFunctor> BindingMap;

  GlfBindingMap() : _samplerBindingBaseIndex(0), _uniformBindingBaseIndex(0) {}

  GLF_API
  int GetSamplerUnit(std::string const &name);
  GLF_API
  int GetSamplerUnit(TfToken const &name);

  // If GetAttributeIndex is called with an unknown
  // attribute token they return -1
  GLF_API
  int GetAttributeIndex(std::string const &name);
  GLF_API
  int GetAttributeIndex(TfToken const &name);

  GLF_API
  int GetUniformBinding(std::string const &name);
  GLF_API
  int GetUniformBinding(TfToken const &name);

  GLF_API
  bool HasUniformBinding(std::string const &name) const;
  GLF_API
  bool HasUniformBinding(TfToken const &name) const;

  int GetNumSamplerBindings() const
  {
    return (int)_samplerBindings.size();
  }

  void ClearAttribBindings()
  {
    _attribBindings.clear();
  }

  /// \name Sampler and UBO Bindings
  ///
  /// Sampler units and uniform block bindings are reset and will be
  /// assigned sequentially starting from the specified baseIndex.
  /// This allows other subsystems to claim sampler units and uniform
  /// block bindings before additional indices are assigned by this
  /// binding map.
  ///
  /// @{

  void ResetSamplerBindings(int baseIndex)
  {
    _samplerBindings.clear();
    _samplerBindingBaseIndex = baseIndex;
  }

  void ResetUniformBindings(int baseIndex)
  {
    _uniformBindings.clear();
    _uniformBindingBaseIndex = baseIndex;
  }

  /// @}

  void AddAttribBinding(TfToken const &name, int location)
  {
    _attribBindings[name] = location;
  }

  BindingMap const &GetAttributeBindings() const
  {
    return _attribBindings;
  }

  GLF_API
  void AssignSamplerUnitsToProgram(GLuint program);

  GLF_API
  void AssignUniformBindingsToProgram(GLuint program);

  GLF_API
  void AddCustomBindings(GLuint program);

  GLF_API
  void Debug() const;

 private:

  void _AddActiveAttributeBindings(GLuint program);
  void _AddActiveUniformBindings(GLuint program);
  void _AddActiveUniformBlockBindings(GLuint program);

  BindingMap _attribBindings;
  BindingMap _samplerBindings;
  BindingMap _uniformBindings;

  int _samplerBindingBaseIndex;
  int _uniformBindingBaseIndex;
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_GLF_BINDING_MAP_H
