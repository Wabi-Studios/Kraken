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
#ifndef WABI_IMAGING_HD_BUFFER_ARRAY_H
#define WABI_IMAGING_HD_BUFFER_ARRAY_H

#include "wabi/base/tf/token.h"
#include "wabi/base/vt/value.h"
#include "wabi/imaging/hd/api.h"
#include "wabi/imaging/hd/bufferResource.h"
#include "wabi/imaging/hd/bufferSpec.h"
#include "wabi/imaging/hd/version.h"
#include "wabi/wabi.h"

#include <atomic>
#include <memory>
#include <mutex>

WABI_NAMESPACE_BEGIN

class HdBufferArrayRange;

using HdBufferArraySharedPtr      = std::shared_ptr<class HdBufferArray>;
using HdBufferArrayRangeSharedPtr = std::shared_ptr<HdBufferArrayRange>;
using HdBufferArrayRangePtr       = std::weak_ptr<HdBufferArrayRange>;

/// \union HdBufferArrayUsageHint
///
/// The union provides a set of flags that provide hints to the memory
/// management system about the properties of a Buffer Array Range (BAR),
/// so it can efficiently organize that memory.  For example,
/// the memory manager should probably not aggregate BARs with different
/// usage hints.
///
/// The union provides two members:
///   - value: The combined set of flags
///   - bits:  Access to individual flag bits
///
/// The flag bits are:
///   - immutable: The BAR will not be modified once created and populated.
///   - sizeVarying: The number of elements in the BAR changes with time.
///
/// Some flag bits may not make sense in combination
/// (i.e. mutually exclusive to each other).  For example, it is logically
/// impossible to be both immutable (i.e. not changing) and sizeVarying
/// (changing).  However, these logically impossible combinations are
/// not enforced and remain valid potential values.
///
union HdBufferArrayUsageHint {
  struct _Bits {
    uint32_t immutable   : 1;
    uint32_t sizeVarying : 1;
    uint32_t pad         : 30;
  } bits;
  uint32_t value;

  HdBufferArrayUsageHint() : value(0)
  {}
};

/// \class HdBufferArray
///
/// Similar to a VAO, this object is a bundle of coherent buffers. This object
/// can be shared across multiple HdRprims, in the context of buffer
/// aggregation.
///
class HdBufferArray : public std::enable_shared_from_this<HdBufferArray> {
 public:
  HD_API
  HdBufferArray(TfToken const &role,
                TfToken const garbageCollectionPerfToken,
                HdBufferArrayUsageHint usageHint);

  HD_API
  virtual ~HdBufferArray();

  /// Returns the role of the GPU data in this bufferArray.
  TfToken const &GetRole() const
  {
    return _role;
  }

  /// Returns the version of this buffer array.
  /// Used to determine when to rebuild outdated indirect dispatch buffers
  size_t GetVersion() const
  {
    return _version;
  }

  /// Increments the version of this buffer array.
  HD_API
  void IncrementVersion();

  /// Attempts to assign a range to this buffer array.
  /// Multiple threads could be trying to assign to this buffer at the same time.
  /// Returns true is the range is assigned to this buffer otherwise
  /// returns false if the buffer doesn't have space to assign the range.
  HD_API
  bool TryAssignRange(HdBufferArrayRangeSharedPtr &range);

  /// Performs compaction if necessary and returns true if it becomes empty.
  virtual bool GarbageCollect() = 0;

  /// Performs reallocation. After reallocation, the buffer will contain
  /// the specified \a ranges. If these ranges are currently held by a
  /// different buffer array instance, then their data will be copied
  /// from the specified \a curRangeOwner.
  virtual void Reallocate(std::vector<HdBufferArrayRangeSharedPtr> const &ranges,
                          HdBufferArraySharedPtr const &curRangeOwner) = 0;

  /// Returns the maximum number of elements capacity.
  HD_API
  virtual size_t GetMaxNumElements() const;

  /// Debug output
  virtual void DebugDump(std::ostream &out) const = 0;

  /// How many ranges are attached to the buffer array.
  size_t GetRangeCount() const
  {
    return _rangeCount;
  }

  /// Get the attached range at the specified index.
  HD_API
  HdBufferArrayRangePtr GetRange(size_t idx) const;

  /// Remove any ranges from the range list that have been deallocated
  /// Returns number of ranges after clean-up
  HD_API
  void RemoveUnusedRanges();

  /// Returns true if Reallocate() needs to be called on this buffer array.
  bool NeedsReallocation() const
  {
    return _needsReallocation;
  }

  /// Returns true if this buffer array is marked as immutable.
  bool IsImmutable() const
  {
    return _usageHint.bits.immutable;
  }

  /// Returns the usage hints for this buffer array.
  HdBufferArrayUsageHint GetUsageHint() const
  {
    return _usageHint;
  }

 protected:
  /// Dirty bit to set when the ranges attached to the buffer
  /// changes.  If set Reallocate() should be called to clean it.
  bool _needsReallocation;

  /// Limits the number of ranges that can be
  /// allocated to this buffer to max.
  void _SetMaxNumRanges(size_t max)
  {
    _maxNumRanges = max;
  }

  /// Swap the rangelist with \p ranges
  HD_API
  void _SetRangeList(std::vector<HdBufferArrayRangeSharedPtr> const &ranges);

 private:
  // Do not allow copies.
  HdBufferArray(const HdBufferArray &) = delete;
  HdBufferArray &operator=(const HdBufferArray &) = delete;

  typedef std::vector<HdBufferArrayRangePtr> _RangeList;

  // Vector of ranges associated with this buffer
  // We add values to the list in a multi-threaded fashion
  // but can later remove them in _RemoveUnusedRanges
  // than add more.
  //
  _RangeList _rangeList;
  std::atomic_size_t _rangeCount;  // how many ranges are valid in list
  std::mutex _rangeListLock;

  const TfToken _role;
  const TfToken _garbageCollectionPerfToken;

  size_t _version;

  size_t _maxNumRanges;
  HdBufferArrayUsageHint _usageHint;
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HD_BUFFER_ARRAY_H
