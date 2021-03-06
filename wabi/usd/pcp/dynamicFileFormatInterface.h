//
// Copyright 2019 Pixar
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
#ifndef WABI_USD_PCP_DYNAMIC_FILE_FORMAT_INTERFACE_H
#define WABI_USD_PCP_DYNAMIC_FILE_FORMAT_INTERFACE_H

#include "wabi/wabi.h"
#include "wabi/usd/pcp/api.h"
#include "wabi/usd/sdf/fileFormat.h"

WABI_NAMESPACE_BEGIN

class PcpDynamicFileFormatContext;
class VtValue;

/// \class PcpDynamicFileFormatInterface
///
/// Interface mixin that can be included by SdfFileFormat subclasses to enable
/// dynamic file format argument generation for a file format plugin.
/// When prim index composition encounters a payload to an asset of a file
/// format that implements this interface, it will call
/// ComposeFieldsForFileFormatArguments to generate arguments from the current
/// composition context at which the payload is being added. The derived file
/// format is on the hook for using the provided context to compute any prim
/// field values it needs and generate the relevant file format arguments for
/// its content.
class PcpDynamicFileFormatInterface
{
 public:

  /// Empty virtual destructor to prevent build errors with some compilers.
  PCP_API
  virtual ~PcpDynamicFileFormatInterface();

  /// Derived classes must implement this function to compose prim fields
  /// using the given \p context and use them to generate file format
  /// arguments for the layer at \p assetPath. The context provides methods
  /// to compose field values at the current point in prim index composition
  /// which can be used to generate the relevant file format arguments. These
  /// arguments need to be added to the set of file format arguments provided
  /// by \p args.
  ///
  /// Additionally, implementations can output \p dependencyContextData of
  /// any value type that will then be passed back in to calls to
  /// CanFieldChangeAffectFileFormatArguments during change processing.
  /// This can be used to provide more that context that is specific to the
  /// file format when determining whether a field change really does affect
  /// the arguments generated by a particular call to this function.
  PCP_API
  virtual void ComposeFieldsForFileFormatArguments(const std::string &assetPath,
                                                   const PcpDynamicFileFormatContext &context,
                                                   SdfFileFormat::FileFormatArguments *args,
                                                   VtValue *dependencyContextData) const = 0;

  /// Derived classes must implement this function to return true if the
  /// change to scene description of the field named \p field can affect the
  /// dynamic file format arguments generated by
  /// ComposeFieldsForFileFormatArguments. This function will be called during
  /// change processing to determine whether a change to a field affects
  /// the dynamic file format arguments of a payload that a prim index depends
  /// on.
  ///
  /// \p oldValue and \p newValue contain the old and new values of the field.
  /// \p dependencyContextData is the arbitrary typed data that was generated
  /// by the call to ComposeFieldsForFileFormatArguments when the payload arc
  /// was computed.
  PCP_API
  virtual bool CanFieldChangeAffectFileFormatArguments(
    const TfToken &field,
    const VtValue &oldValue,
    const VtValue &newValue,
    const VtValue &dependencyContextData) const = 0;
};

WABI_NAMESPACE_END

#endif  // WABI_USD_PCP_DYNAMIC_FILE_FORMAT_INTERFACE_H
