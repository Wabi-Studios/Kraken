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
#include "wabi/base/tf/diagnostic.h"
#include "wabi/base/tf/envSetting.h"
#include "wabi/base/tf/iterator.h"

#include "wabi/imaging/hdPh/bufferResource.h"
#include "wabi/imaging/hdPh/glUtils.h"
#include "wabi/imaging/hdPh/tokens.h"
#include "wabi/imaging/hdPh/vboSimpleMemoryManager.h"

#include "wabi/imaging/hd/bufferArrayRange.h"
#include "wabi/imaging/hd/bufferSource.h"
#include "wabi/imaging/hd/perfLog.h"
#include "wabi/imaging/hd/tokens.h"

#include "wabi/imaging/hgi/blitCmds.h"
#include "wabi/imaging/hgi/blitCmdsOps.h"
#include "wabi/imaging/hgi/buffer.h"
#include "wabi/imaging/hgi/hgi.h"

#include "wabi/imaging/hf/perfLog.h"

#include <atomic>

#include <boost/functional/hash.hpp>

WABI_NAMESPACE_BEGIN

extern TfEnvSetting<int> HD_MAX_VBO_SIZE;

// ---------------------------------------------------------------------------
//  HdPhVBOSimpleMemoryManager
// ---------------------------------------------------------------------------

HdBufferArraySharedPtr HdPhVBOSimpleMemoryManager::CreateBufferArray(
  TfToken const &role,
  HdBufferSpecVector const &bufferSpecs,
  HdBufferArrayUsageHint usageHint)
{
  return std::make_shared<HdPhVBOSimpleMemoryManager::_SimpleBufferArray>(_resourceRegistry,
                                                                          role,
                                                                          bufferSpecs,
                                                                          usageHint);
}

HdBufferArrayRangeSharedPtr HdPhVBOSimpleMemoryManager::CreateBufferArrayRange()
{
  return std::make_shared<HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange>(_resourceRegistry);
}

HdAggregationStrategy::AggregationId HdPhVBOSimpleMemoryManager::ComputeAggregationId(
  HdBufferSpecVector const &bufferSpecs,
  HdBufferArrayUsageHint usageHint) const
{
  // Always returns different value
  static std::atomic_uint id(0);

  AggregationId hash = id++;  // Atomic

  return hash;
}

/// Returns the buffer specs from a given buffer array
HdBufferSpecVector HdPhVBOSimpleMemoryManager::GetBufferSpecs(
  HdBufferArraySharedPtr const &bufferArray) const
{
  _SimpleBufferArraySharedPtr bufferArray_ = std::static_pointer_cast<_SimpleBufferArray>(
    bufferArray);
  return bufferArray_->GetBufferSpecs();
}

/// Returns the size of the GPU memory used by the passed buffer array
size_t HdPhVBOSimpleMemoryManager::GetResourceAllocation(HdBufferArraySharedPtr const &bufferArray,
                                                         VtDictionary &result) const
{
  std::set<uint64_t> idSet;
  size_t gpuMemoryUsed = 0;

  _SimpleBufferArraySharedPtr bufferArray_ = std::static_pointer_cast<_SimpleBufferArray>(
    bufferArray);

  TF_FOR_ALL (resIt, bufferArray_->GetResources()) {
    HdPhBufferResourceSharedPtr const &resource = resIt->second;

    // XXX Reallocate inserts an empty (invalid) handle for empty buffers.
    HgiBufferHandle buffer = resource->GetHandle();
    uint64_t id = buffer ? buffer->GetRawResource() : 0;

    // XXX avoid double counting of resources shared within a buffer
    if (id > 0 && idSet.count(id) == 0) {
      idSet.insert(id);

      std::string const &role = resource->GetRole().GetString();
      size_t size = size_t(resource->GetSize());

      if (result.count(role)) {
        size_t currentSize = result[role].Get<size_t>();
        result[role] = VtValue(currentSize + size);
      } else {
        result[role] = VtValue(size);
      }

      gpuMemoryUsed += size;
    }
  }

  return gpuMemoryUsed;
}

// ---------------------------------------------------------------------------
//  _SimpleBufferArray
// ---------------------------------------------------------------------------
HdPhVBOSimpleMemoryManager::_SimpleBufferArray::_SimpleBufferArray(
  HdPhResourceRegistry *resourceRegistry,
  TfToken const &role,
  HdBufferSpecVector const &bufferSpecs,
  HdBufferArrayUsageHint usageHint)
  : HdBufferArray(role, TfToken(), usageHint),
    _resourceRegistry(resourceRegistry),
    _capacity(0),
    _maxBytesPerElement(0)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  // populate BufferResources
  TF_FOR_ALL (it, bufferSpecs) {
    int stride = HdDataSizeOfTupleType(it->tupleType);
    _AddResource(it->name, it->tupleType, /*offset=*/0, stride);
  }

  _SetMaxNumRanges(1);

  // compute max bytes / elements
  TF_FOR_ALL (it, GetResources()) {
    HdPhBufferResourceSharedPtr const &bres = it->second;
    _maxBytesPerElement = std::max(_maxBytesPerElement,
                                   HdDataSizeOfTupleType(bres->GetTupleType()));
  }
}

HdPhBufferResourceSharedPtr HdPhVBOSimpleMemoryManager::_SimpleBufferArray::_AddResource(
  TfToken const &name,
  HdTupleType tupleType,
  int offset,
  int stride)
{
  HD_TRACE_FUNCTION();
  if (TfDebug::IsEnabled(HD_SAFE_MODE)) {
    // duplication check
    HdPhBufferResourceSharedPtr bufferRes = GetResource(name);
    if (!TF_VERIFY(!bufferRes)) {
      return bufferRes;
    }
  }

  HdPhBufferResourceSharedPtr bufferRes = std::make_shared<HdPhBufferResource>(GetRole(),
                                                                               tupleType,
                                                                               offset,
                                                                               stride);
  _resourceList.emplace_back(name, bufferRes);
  return bufferRes;
}

HdPhVBOSimpleMemoryManager::_SimpleBufferArray::~_SimpleBufferArray()
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  // invalidate buffer array range
  // (the range may still be held by drawItems)
  _SimpleBufferArrayRangeSharedPtr range = _GetRangeSharedPtr();
  if (range) {
    range->Invalidate();
  }
}

bool HdPhVBOSimpleMemoryManager::_SimpleBufferArray::GarbageCollect()
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  // no range referring this buffer = empty
  if (GetRangeCount() > 0 && GetRange(0).expired()) {
    _DeallocateResources();
    HD_PERF_COUNTER_INCR(HdPerfTokens->garbageCollectedVbo);
    return true;
  }
  return false;
}

void HdPhVBOSimpleMemoryManager::_SimpleBufferArray::DebugDump(std::ostream &out) const
{
  out << "  HdPhVBOSimpleMemoryManager";
  out << "  total capacity = " << _capacity << "\n";
}

bool HdPhVBOSimpleMemoryManager::_SimpleBufferArray::Resize(int numElements)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  // see the comment in
  // HdPhVBOMemoryManager::_StripedBufferArrayRange::Resize(int numElements)
  // this change is for the unit test consistency.
  //
  // if (_capacity < numElements) {
  if (_capacity != numElements) {
    _needsReallocation = true;
    return true;
  }
  return false;
}

void HdPhVBOSimpleMemoryManager::_SimpleBufferArray::Reallocate(
  std::vector<HdBufferArrayRangeSharedPtr> const &ranges,
  HdBufferArraySharedPtr const &curRangeOwner)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  HD_PERF_COUNTER_INCR(HdPerfTokens->vboRelocated);

  if (!TF_VERIFY(curRangeOwner == shared_from_this())) {
    TF_CODING_ERROR("HdPhVBOSimpleMemoryManager can't reassign ranges");
    return;
  }

  if (ranges.size() > 1) {
    TF_CODING_ERROR("HdPhVBOSimpleMemoryManager can't take multiple ranges");
    return;
  }
  _SetRangeList(ranges);

  _SimpleBufferArrayRangeSharedPtr range = _GetRangeSharedPtr();

  if (!range) {
    TF_CODING_ERROR("_SimpleBufferArrayRange expired unexpectedly.");
    return;
  }

  int numElements = range->GetNumElements();

  // Use blit work to record resource copy commands.
  Hgi *hgi = _resourceRegistry->GetHgi();
  HgiBlitCmds *blitCmds = _resourceRegistry->GetGlobalBlitCmds();
  blitCmds->PushDebugGroup(__ARCH_PRETTY_FUNCTION__);

  TF_FOR_ALL (bresIt, GetResources()) {
    HdPhBufferResourceSharedPtr const &bres = bresIt->second;

    size_t bytesPerElement = HdDataSizeOfTupleType(bres->GetTupleType());
    size_t bufferSize = bytesPerElement * numElements;

    HgiBufferHandle oldBuf = bres->GetHandle();
    HgiBufferHandle newBuf;

    if (bufferSize > 0) {
      HgiBufferDesc bufDesc;
      bufDesc.byteSize = bufferSize;
      bufDesc.usage = HgiBufferUsageUniform;
      newBuf = hgi->CreateBuffer(bufDesc);
    }

    // copy the range. There are three cases:
    //
    // 1. src length (capacity) == dst length (numElements)
    //   Copy the entire range
    //
    // 2. src length < dst length
    //   Enlarging the range. This typically happens when
    //   applying quadrangulation/subdivision to populate
    //   additional data at the end of source data.
    //
    // 3. src length > dst length
    //   Shrinking the range. When the garbage collection
    //   truncates ranges.
    //
    int oldSize = range->GetCapacity();
    int newSize = range->GetNumElements();
    size_t copySize = std::min(oldSize, newSize) * bytesPerElement;
    if (copySize > 0 && oldBuf) {
      HD_PERF_COUNTER_INCR(HdPhPerfTokens->copyBufferGpuToGpu);

      HgiBufferGpuToGpuOp blitOp;
      blitOp.gpuSourceBuffer = oldBuf;
      blitOp.gpuDestinationBuffer = newBuf;
      blitOp.byteSize = copySize;
      blitCmds->CopyBufferGpuToGpu(blitOp);
    }

    // delete old buffer
    if (oldBuf) {
      hgi->DestroyBuffer(&oldBuf);
    }

    bres->SetAllocation(newBuf, bufferSize);
  }

  blitCmds->PopDebugGroup();

  _capacity = numElements;
  _needsReallocation = false;

  // increment version to rebuild dispatch buffers.
  IncrementVersion();
}

size_t HdPhVBOSimpleMemoryManager::_SimpleBufferArray::GetMaxNumElements() const
{
  static size_t vboMaxSize = TfGetEnvSetting(HD_MAX_VBO_SIZE);
  return vboMaxSize / _maxBytesPerElement;
}

void HdPhVBOSimpleMemoryManager::_SimpleBufferArray::_DeallocateResources()
{
  Hgi *hgi = _resourceRegistry->GetHgi();
  TF_FOR_ALL (it, GetResources()) {
    hgi->DestroyBuffer(&it->second->GetHandle());
  }
}

HdPhBufferResourceSharedPtr HdPhVBOSimpleMemoryManager::_SimpleBufferArray::GetResource() const
{
  HD_TRACE_FUNCTION();

  if (_resourceList.empty())
    return HdPhBufferResourceSharedPtr();

  if (TfDebug::IsEnabled(HD_SAFE_MODE)) {
    // make sure this buffer array has only one resource.
    HgiBufferHandle const &buffer = _resourceList.begin()->second->GetHandle();
    TF_FOR_ALL (it, _resourceList) {
      if (it->second->GetHandle() != buffer) {
        TF_CODING_ERROR(
          "GetResource(void) called on"
          "HdBufferArray having multiple GPU resources");
      }
    }
  }

  // returns the first item
  return _resourceList.begin()->second;
}

HdPhBufferResourceSharedPtr HdPhVBOSimpleMemoryManager::_SimpleBufferArray::GetResource(
  TfToken const &name)
{
  HD_TRACE_FUNCTION();

  // linear search.
  // The number of buffer resources should be small (<10 or so).
  for (HdPhBufferResourceNamedList::iterator it = _resourceList.begin(); it != _resourceList.end();
       ++it) {
    if (it->first == name)
      return it->second;
  }
  return HdPhBufferResourceSharedPtr();
}

HdBufferSpecVector HdPhVBOSimpleMemoryManager::_SimpleBufferArray::GetBufferSpecs() const
{
  HdBufferSpecVector result;
  result.reserve(_resourceList.size());
  TF_FOR_ALL (it, _resourceList) {
    result.emplace_back(it->first, it->second->GetTupleType());
  }
  return result;
}

// ---------------------------------------------------------------------------
//  _SimpleBufferArrayRange
// ---------------------------------------------------------------------------
bool HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange::IsAssigned() const
{
  return (_bufferArray != nullptr);
}

bool HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange::IsImmutable() const
{
  return (_bufferArray != nullptr) && _bufferArray->IsImmutable();
}

void HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange::CopyData(
  HdBufferSourceSharedPtr const &bufferSource)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  if (!TF_VERIFY(_bufferArray))
    return;

  int offset = 0;

  HdPhBufferResourceSharedPtr VBO = _bufferArray->GetResource(bufferSource->GetName());

  if (!VBO || !VBO->GetHandle()) {
    TF_CODING_ERROR("VBO doesn't exist for %s", bufferSource->GetName().GetText());
    return;
  }

  size_t bytesPerElement = HdDataSizeOfTupleType(VBO->GetTupleType());
  // overrun check. for graceful handling of erroneous assets,
  // issue warning here and continue to copy for the valid range.
  size_t dstSize = _numElements * bytesPerElement;
  size_t srcSize = bufferSource->GetNumElements() *
                   HdDataSizeOfTupleType(bufferSource->GetTupleType());
  if (srcSize > dstSize) {
    TF_WARN("%s: size %ld is larger than the range (%ld)",
            bufferSource->GetName().GetText(),
            srcSize,
            dstSize);
    srcSize = dstSize;
  }

  size_t vboOffset = bytesPerElement * offset;

  HD_PERF_COUNTER_INCR(HdPhPerfTokens->copyBufferCpuToGpu);

  HgiBufferCpuToGpuOp blitOp;
  blitOp.cpuSourceBuffer = bufferSource->GetData();
  blitOp.gpuDestinationBuffer = VBO->GetHandle();

  blitOp.sourceByteOffset = 0;
  blitOp.byteSize = srcSize;
  blitOp.destinationByteOffset = vboOffset;

  HgiBlitCmds *blitCmds = GetResourceRegistry()->GetGlobalBlitCmds();
  blitCmds->CopyBufferCpuToGpu(blitOp);
}

VtValue HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange::ReadData(TfToken const &name) const
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  if (!TF_VERIFY(_bufferArray))
    return VtValue();

  HdPhBufferResourceSharedPtr VBO = _bufferArray->GetResource(name);

  if (!VBO || (!VBO->GetHandle() && _numElements > 0)) {
    TF_CODING_ERROR("VBO doesn't exist for %s", name.GetText());
    return VtValue();
  }

  return HdPhGLUtils::ReadBuffer(VBO->GetHandle()->GetRawResource(),
                                 VBO->GetTupleType(),
                                 /*offset=*/0,
                                 /*stride=*/0,  // not interleaved.
                                 _numElements);
}

size_t HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange::GetMaxNumElements() const
{
  return _bufferArray->GetMaxNumElements();
}

HdBufferArrayUsageHint HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange::GetUsageHint() const
{
  if (!TF_VERIFY(_bufferArray)) {
    return HdBufferArrayUsageHint();
  }

  return _bufferArray->GetUsageHint();
}

HdPhBufferResourceSharedPtr HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange::GetResource()
  const
{
  if (!TF_VERIFY(_bufferArray))
    return HdPhBufferResourceSharedPtr();

  return _bufferArray->GetResource();
}

HdPhBufferResourceSharedPtr HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange::GetResource(
  TfToken const &name)
{
  if (!TF_VERIFY(_bufferArray))
    return HdPhBufferResourceSharedPtr();
  return _bufferArray->GetResource(name);
}

HdPhBufferResourceNamedList const &HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange::
  GetResources() const
{
  if (!TF_VERIFY(_bufferArray)) {
    static HdPhBufferResourceNamedList empty;
    return empty;
  }
  return _bufferArray->GetResources();
}

void HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange::SetBufferArray(
  HdBufferArray *bufferArray)
{
  _bufferArray = static_cast<_SimpleBufferArray *>(bufferArray);
}

void HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange::DebugDump(std::ostream &out) const
{
  out << "[SimpleBAR] numElements = " << _numElements << "\n";
}

const void *HdPhVBOSimpleMemoryManager::_SimpleBufferArrayRange::_GetAggregation() const
{
  return _bufferArray;
}

WABI_NAMESPACE_END
