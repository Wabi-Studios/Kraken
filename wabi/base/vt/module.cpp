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

#include "wabi/wabi.h"
#include "wabi/base/tf/pyModule.h"

WABI_NAMESPACE_BEGIN

// Defined in arrayPyBuffer.cpp
void Vt_AddBufferProtocolSupportToVtArrays();

WABI_NAMESPACE_END

WABI_NAMESPACE_USING

TF_WRAP_MODULE
{
  TF_WRAP(Array);
  TF_WRAP(ArrayFloat);
  TF_WRAP(ArrayIntegral);
  TF_WRAP(ArrayMatrix);
  TF_WRAP(ArrayQuaternion);
  TF_WRAP(ArrayRange);
  TF_WRAP(ArrayString);
  TF_WRAP(ArrayToken);
  TF_WRAP(ArrayVec);

  // Add python buffer protocol support to VtArray.
  Vt_AddBufferProtocolSupportToVtArrays();

  TF_WRAP(Dictionary);
  TF_WRAP(Value);
}
