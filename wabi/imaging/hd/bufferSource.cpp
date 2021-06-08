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
#include "wabi/imaging/hd/bufferSource.h"

#include "wabi/base/arch/hash.h"

WABI_NAMESPACE_BEGIN

template<class HashState> void TfHashAppend(HashState &h, HdBufferSource const &bs)
{
  HdTupleType tt = bs.GetTupleType();
  h.AppendContiguous(reinterpret_cast<const char *>(bs.GetData()),
                     HdDataSizeOfTupleType(tt) * bs.GetNumElements());
  // Hash signature as well.
  h.Append(bs.GetName(), tt);
}

HdBufferSource::~HdBufferSource()
{}

size_t HdBufferSource::ComputeHash() const
{
  return TfHash()(*this);
}

bool HdBufferSource::HasPreChainedBuffer() const
{
  return false;
}

HdBufferSourceSharedPtr HdBufferSource::GetPreChainedBuffer() const
{
  return HdBufferSourceSharedPtr();
}

bool HdBufferSource::HasChainedBuffer() const
{
  return false;
}

HdBufferSourceSharedPtrVector HdBufferSource::GetChainedBuffers() const
{
  return HdBufferSourceSharedPtrVector();
}

bool HdBufferSource::IsValid() const
{
  return _CheckValid();
}

// ---------------------------------------------------------------------------

size_t HdComputedBufferSource::ComputeHash() const
{
  return 0;
}

TfToken const &HdComputedBufferSource::GetName() const
{
  if (!_result) {
    static TfToken empty;
    return empty;
  }
  return _result->GetName();
}

void const *HdComputedBufferSource::GetData() const
{
  if (!_result) {
    TF_CODING_ERROR(
        "HdComputedBufferSource::GetData() called without "
        "setting the result.");
    return nullptr;
  }
  return _result->GetData();
}

HdTupleType HdComputedBufferSource::GetTupleType() const
{
  if (!_result) {
    TF_CODING_ERROR(
        "HdComputedBufferSource::GetTupleType() called "
        "without setting the result.");
    return {HdTypeInvalid, 0};
  }
  return _result->GetTupleType();
}

size_t HdComputedBufferSource::GetNumElements() const
{
  // GetNumElements returns 0 for the empty result.
  return _result ? _result->GetNumElements() : 0;
}

// ---------------------------------------------------------------------------

size_t HdNullBufferSource::ComputeHash() const
{
  return 0;
}

TfToken const &HdNullBufferSource::GetName() const
{
  TF_CODING_ERROR("HdNullBufferSource can't be scheduled with a buffer range");
  static TfToken empty;
  return empty;
}

void const *HdNullBufferSource::GetData() const
{
  TF_CODING_ERROR("HdNullBufferSource can't be scheduled with a buffer range");
  return nullptr;
}

HdTupleType HdNullBufferSource::GetTupleType() const
{
  TF_CODING_ERROR(
      "HdNullBufferSource can't be scheduled with a buffer "
      "range");
  return {HdTypeInvalid, 0};
}

size_t HdNullBufferSource::GetNumElements() const
{
  TF_CODING_ERROR("HdNullBufferSource can't be scheduled with a buffer range");
  return 0;
}

void HdNullBufferSource::GetBufferSpecs(HdBufferSpecVector *specs) const
{
  // nothing
}

WABI_NAMESPACE_END
