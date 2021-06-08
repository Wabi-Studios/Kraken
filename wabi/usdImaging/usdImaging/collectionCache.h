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
#ifndef WABI_USD_IMAGING_USD_IMAGING_COLLECTION_CACHE_H
#define WABI_USD_IMAGING_USD_IMAGING_COLLECTION_CACHE_H

/// \file usdImaging/collectionCache.h

#include "wabi/usd/usd/collectionAPI.h"
#include "wabi/usdImaging/usdImaging/api.h"
#include "wabi/wabi.h"

#include <boost/noncopyable.hpp>

#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_unordered_map.h>

#include <unordered_map>

WABI_NAMESPACE_BEGIN

/// \class UsdImaging_CollectionCache
///
/// UsdImaging_CollectionCache provides a cache of Usd collections
/// discovered in the scene.  It associates the collection paths
/// (UsdCollectionAPI::GetCollectionPath) with the computed
/// membership query (UsdCollectionAPI::ComputeMembershipQuery).
///
/// For efficiency, it groups collections into equivalence
/// classes based on the computed query.  Collections that yield
/// equivalent queries are merged.  Each unique query is assigned
/// an identifier token.
///
/// This cache is used to track the collections used for linking
/// UsdLux lights/shadows/filters to geometry.
///
/// As an optimization, the query that includes everything is
/// treated as a special case and given the empty id, TfToken().
///
class UsdImaging_CollectionCache : boost::noncopyable {
 public:
  /// Query is the MembershipQuery computed from a collection's state.
  typedef UsdCollectionAPI::MembershipQuery Query;

  /// Computes the membership query from the current state of the
  /// given collection, and establishes a cache entry.  If a
  /// prior entry existed for the collection at this path,
  /// it is removed first.
  TfToken UpdateCollection(UsdCollectionAPI const &collection);

  /// Remove any cached entry for the given collection.
  /// Does nothing if no cache entry exists.
  void RemoveCollection(UsdCollectionAPI const &collection);

  /// Return the cached entry for the given collection.
  TfToken GetIdForCollection(UsdCollectionAPI const &collection);

  /// Return a list of identifiers of all collections that contain
  // the given path.
  VtArray<TfToken> ComputeCollectionsContainingPath(SdfPath const &path) const;

 private:
  // The cache boils down to tracking the correspondence of
  // collection paths, their computed queries, and the id
  // assigned to each unique query:
  //
  // CollectionPath <=> MembershipQuery <=> AssignedId
  //
  // In this scheme, the assigned id provides a compact but
  // potentially human-meaningful reference to the query,
  // which we can pass to the renderer.
  //
  std::unordered_map<Query, TfToken, Query::Hash> _idForQuery;
  std::unordered_map<TfToken, Query, TfToken::HashFunctor> _queryForId;
  std::unordered_map<SdfPath, TfToken, SdfPath::Hash> _idForPath;
  std::unordered_map<Query, SdfPathSet, Query::Hash> _pathsForQuery;

  std::mutex _mutex;
};

WABI_NAMESPACE_END

#endif  // WABI_USD_IMAGING_USD_IMAGING_COLLECTION_CACHE_H
