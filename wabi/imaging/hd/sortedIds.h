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
#ifndef WABI_IMAGING_HD_SORTED_IDS_H
#define WABI_IMAGING_HD_SORTED_IDS_H

#include "wabi/imaging/hd/api.h"
#include "wabi/wabi.h"

#include "wabi/usd/sdf/path.h"

WABI_NAMESPACE_BEGIN

///
/// Manages a container of Hydra Ids in a sorted order.
///
/// For performance reasons, sorting of the list is deferred
/// due to inserting a large number of items at once.
///
/// The class chooses the type of sort based on how many unsorted items
/// there are in the list.
///
class Hd_SortedIds {
 public:
  HD_API
  Hd_SortedIds();

  HD_API
  ~Hd_SortedIds() = default;

  HD_API
  Hd_SortedIds(Hd_SortedIds &&other);

  /// Sorts the ids if needed and returns the sorted list of ids.
  HD_API
  const SdfPathVector &GetIds();

  /// Add a new id to the collection
  HD_API
  void Insert(const SdfPath &id);

  /// Remove an id from the collection.
  HD_API
  void Remove(const SdfPath &id);

  /// Remove a range of id from the collection.
  /// Range defined by position index in sorted list.
  /// end is inclusive.
  HD_API
  void RemoveRange(size_t start, size_t end);

  /// Removes all ids from the collection.
  HD_API
  void Clear();

 private:
  SdfPathVector _ids;
  size_t _sortedCount;
  ptrdiff_t _afterLastDeletePoint;

  void _InsertSort();
  void _FullSort();
  void _Sort();

  Hd_SortedIds(const Hd_SortedIds &) = delete;
  Hd_SortedIds &operator=(const Hd_SortedIds &) = delete;
};

WABI_NAMESPACE_END

#endif  // WABI_IMAGING_HD_SORTED_IDS_H
