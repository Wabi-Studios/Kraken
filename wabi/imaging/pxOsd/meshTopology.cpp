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
///
/// \file pxOsd/meshTopology.h
///

#include "wabi/imaging/pxOsd/meshTopology.h"
#include "wabi/imaging/pxOsd/meshTopologyValidation.h"
#include "wabi/imaging/pxOsd/tokens.h"

#include "wabi/base/arch/hash.h"
#include "wabi/base/tf/debug.h"
#include "wabi/base/tf/diagnostic.h"
#include "wabi/base/trace/trace.h"

WABI_NAMESPACE_BEGIN

PxOsdMeshTopology::PxOsdMeshTopology()
  : _scheme(PxOsdOpenSubdivTokens->bilinear),
    _orientation(PxOsdOpenSubdivTokens->rightHanded),
    _faceVertexCounts(),
    _faceVertexIndices(),
    _holeIndices(),
    _subdivTags()
{}

PxOsdMeshTopology::PxOsdMeshTopology(TfToken const &scheme,
                                     TfToken const &orientation,
                                     VtIntArray const &faceVertexCounts,
                                     VtIntArray const &faceVertexIndices)
  : _scheme(scheme),
    _orientation(orientation),
    _faceVertexCounts(faceVertexCounts),
    _faceVertexIndices(faceVertexIndices),
    _holeIndices(),
    _subdivTags()
{}

PxOsdMeshTopology::PxOsdMeshTopology(TfToken const &scheme,
                                     TfToken const &orientation,
                                     VtIntArray const &faceVertexCounts,
                                     VtIntArray const &faceVertexIndices,
                                     VtIntArray const &holeIndices)
  : _scheme(scheme),
    _orientation(orientation),
    _faceVertexCounts(faceVertexCounts),
    _faceVertexIndices(faceVertexIndices),
    _holeIndices(holeIndices),
    _subdivTags()
{}

PxOsdMeshTopology::PxOsdMeshTopology(TfToken const &scheme,
                                     TfToken const &orientation,
                                     VtIntArray const &faceVertexCounts,
                                     VtIntArray const &faceVertexIndices,
                                     VtIntArray const &holeIndices,
                                     PxOsdSubdivTags const &subdivTags)
  : _scheme(scheme),
    _orientation(orientation),
    _faceVertexCounts(faceVertexCounts),
    _faceVertexIndices(faceVertexIndices),
    _holeIndices(holeIndices),
    _subdivTags(subdivTags)
{}

PxOsdMeshTopology::PxOsdMeshTopology(TfToken const &scheme,
                                     TfToken const &orientation,
                                     VtIntArray const &faceVertexCounts,
                                     VtIntArray const &faceVertexIndices,
                                     PxOsdSubdivTags const &subdivTags)
  : _scheme(scheme),
    _orientation(orientation),
    _faceVertexCounts(faceVertexCounts),
    _faceVertexIndices(faceVertexIndices),
    _holeIndices(),
    _subdivTags(subdivTags)
{}

PxOsdMeshTopology::ID PxOsdMeshTopology::ComputeHash() const
{

  TRACE_FUNCTION();

  ID hash = _subdivTags.ComputeHash();
  hash = ArchHash64((const char *)&_scheme, sizeof(TfToken), hash);
  hash = ArchHash64((const char *)&_orientation, sizeof(TfToken), hash);
  hash = ArchHash64((const char *)_faceVertexCounts.cdata(),
                    _faceVertexCounts.size() * sizeof(int),
                    hash);
  hash = ArchHash64((const char *)_faceVertexIndices.cdata(),
                    _faceVertexIndices.size() * sizeof(int),
                    hash);
  hash = ArchHash64((const char *)_holeIndices.cdata(), _holeIndices.size() * sizeof(int), hash);
  return hash;
}

bool PxOsdMeshTopology::operator==(PxOsdMeshTopology const &other) const
{

  TRACE_FUNCTION();

  return (_scheme == other._scheme && _orientation == other._orientation &&
          _faceVertexCounts == other._faceVertexCounts &&
          _faceVertexIndices == other._faceVertexIndices && _subdivTags == other._subdivTags &&
          _holeIndices == other._holeIndices);
}

PxOsdMeshTopologyValidation PxOsdMeshTopology::Validate() const
{
  TRACE_FUNCTION();
  if (_validated.value) {
    return PxOsdMeshTopologyValidation();
  }

  PxOsdMeshTopologyValidation validation(*this);
  _validated.value.store(bool(validation));
  return validation;
}

std::ostream &operator<<(std::ostream &out, PxOsdMeshTopology const &topo)
{
  out << "(" << topo.GetOrientation().GetString() << ", " << topo.GetScheme().GetString() << ", ("
      << topo.GetFaceVertexCounts() << "), (" << topo.GetFaceVertexIndices() << "), ("
      << topo.GetHoleIndices() << "))";
  return out;
}

bool operator!=(const PxOsdMeshTopology &lhs, const PxOsdMeshTopology &rhs)
{
  return !(lhs == rhs);
}

WABI_NAMESPACE_END
