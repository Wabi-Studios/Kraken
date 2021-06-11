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

#include "wabi/imaging/hdPh/meshTopology.h"
#include "wabi/imaging/hdPh/triangulate.h"

#include "wabi/imaging/hd/meshUtil.h"
#include "wabi/imaging/hd/perfLog.h"
#include "wabi/imaging/hd/tokens.h"
#include "wabi/imaging/hd/vtBufferSource.h"

#include "wabi/base/gf/vec3i.h"

WABI_NAMESPACE_BEGIN

HdPh_TriangleIndexBuilderComputation::HdPh_TriangleIndexBuilderComputation(
    HdPh_MeshTopology *topology,
    SdfPath const &id)
    : _id(id),
      _topology(topology)
{}

void HdPh_TriangleIndexBuilderComputation::GetBufferSpecs(HdBufferSpecVector *specs) const
{
  specs->emplace_back(HdTokens->indices, HdTupleType{HdTypeInt32Vec3, 1});
  // triangles don't support ptex indexing (at least for now).
  specs->emplace_back(HdTokens->primitiveParam, HdTupleType{HdTypeInt32, 1});
  // 1 edge index per triangle
  specs->emplace_back(HdTokens->edgeIndices, HdTupleType{HdTypeInt32, 1});
}

bool HdPh_TriangleIndexBuilderComputation::Resolve()
{
  if (!_TryLock())
    return false;

  HD_TRACE_FUNCTION();

  VtVec3iArray trianglesFaceVertexIndices;
  VtIntArray primitiveParam;
  VtIntArray trianglesEdgeIndices;

  HdMeshUtil meshUtil(_topology, _id);
  meshUtil.ComputeTriangleIndices(
      &trianglesFaceVertexIndices, &primitiveParam, &trianglesEdgeIndices);

  _SetResult(
      std::make_shared<HdVtBufferSource>(HdTokens->indices, VtValue(trianglesFaceVertexIndices)));

  _primitiveParam.reset(new HdVtBufferSource(HdTokens->primitiveParam, VtValue(primitiveParam)));

  _trianglesEdgeIndices.reset(
      new HdVtBufferSource(HdTokens->edgeIndices, VtValue(trianglesEdgeIndices)));

  _SetResolved();
  return true;
}

bool HdPh_TriangleIndexBuilderComputation::HasChainedBuffer() const
{
  return true;
}

HdBufferSourceSharedPtrVector HdPh_TriangleIndexBuilderComputation::GetChainedBuffers() const
{
  return {_primitiveParam, _trianglesEdgeIndices};
}

bool HdPh_TriangleIndexBuilderComputation::_CheckValid() const
{
  return true;
}

// ---------------------------------------------------------------------------

HdPh_TriangulateFaceVaryingComputation::HdPh_TriangulateFaceVaryingComputation(
    HdPh_MeshTopology *topology,
    HdBufferSourceSharedPtr const &source,
    SdfPath const &id)
    : _id(id),
      _topology(topology),
      _source(source)
{}

bool HdPh_TriangulateFaceVaryingComputation::Resolve()
{
  if (!TF_VERIFY(_source))
    return false;
  if (!_source->IsResolved())
    return false;

  if (!_TryLock())
    return false;

  HD_TRACE_FUNCTION();
  HD_PERF_COUNTER_INCR(HdPerfTokens->triangulateFaceVarying);

  VtValue result;
  HdMeshUtil meshUtil(_topology, _id);
  if (meshUtil.ComputeTriangulatedFaceVaryingPrimvar(
          _source->GetData(), _source->GetNumElements(), _source->GetTupleType().type, &result)) {
    _SetResult(std::make_shared<HdVtBufferSource>(_source->GetName(), result));
  }
  else {
    _SetResult(_source);
  }

  _SetResolved();
  return true;
}

void HdPh_TriangulateFaceVaryingComputation::GetBufferSpecs(HdBufferSpecVector *specs) const
{
  // produces same spec buffer as source
  _source->GetBufferSpecs(specs);
}

bool HdPh_TriangulateFaceVaryingComputation::_CheckValid() const
{
  return (_source->IsValid());
}

WABI_NAMESPACE_END
