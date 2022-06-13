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
#include "wabi/imaging/hdPh/smoothNormals.h"
#include "wabi/imaging/hdPh/bufferArrayRange.h"
#include "wabi/imaging/hdPh/bufferResource.h"
#include "wabi/imaging/hdPh/glslProgram.h"
#include "wabi/imaging/hdPh/resourceRegistry.h"
#include "wabi/imaging/hdPh/tokens.h"

#include "wabi/imaging/hd/perfLog.h"
#include "wabi/imaging/hd/vertexAdjacency.h"
#include "wabi/imaging/hd/vtBufferSource.h"

#include "wabi/imaging/hf/perfLog.h"

#include "wabi/imaging/hgi/computeCmds.h"
#include "wabi/imaging/hgi/computePipeline.h"
#include "wabi/imaging/hgi/hgi.h"
#include "wabi/imaging/hgi/shaderProgram.h"
#include "wabi/imaging/hgi/tokens.h"

#include "wabi/base/vt/array.h"

#include "wabi/base/gf/vec3d.h"
#include "wabi/base/gf/vec3f.h"
#include "wabi/base/tf/hash.h"
#include "wabi/base/tf/token.h"

WABI_NAMESPACE_BEGIN

namespace
{

  enum
  {
    BufferBinding_Uniforms,
    BufferBinding_Points,
    BufferBinding_Normals,
    BufferBinding_Adjacency,
  };

  HgiResourceBindingsSharedPtr _CreateResourceBindings(Hgi *hgi,
                                                       HgiBufferHandle const &points,
                                                       HgiBufferHandle const &normals,
                                                       HgiBufferHandle const &adjacency)
  {
    // Begin the resource set
    HgiResourceBindingsDesc resourceDesc;
    resourceDesc.debugName = "SmoothNormals";

    if (points) {
      HgiBufferBindDesc bufBind0;
      bufBind0.bindingIndex = BufferBinding_Points;
      bufBind0.resourceType = HgiBindResourceTypeStorageBuffer;
      bufBind0.stageUsage = HgiShaderStageCompute;
      bufBind0.offsets.push_back(0);
      bufBind0.buffers.push_back(points);
      resourceDesc.buffers.push_back(std::move(bufBind0));
    }

    if (normals) {
      HgiBufferBindDesc bufBind1;
      bufBind1.bindingIndex = BufferBinding_Normals;
      bufBind1.resourceType = HgiBindResourceTypeStorageBuffer;
      bufBind1.stageUsage = HgiShaderStageCompute;
      bufBind1.offsets.push_back(0);
      bufBind1.buffers.push_back(normals);
      resourceDesc.buffers.push_back(std::move(bufBind1));
    }

    if (adjacency) {
      HgiBufferBindDesc bufBind2;
      bufBind2.bindingIndex = BufferBinding_Adjacency;
      bufBind2.resourceType = HgiBindResourceTypeStorageBuffer;
      bufBind2.stageUsage = HgiShaderStageCompute;
      bufBind2.offsets.push_back(0);
      bufBind2.buffers.push_back(adjacency);
      resourceDesc.buffers.push_back(std::move(bufBind2));
    }

    return std::make_shared<HgiResourceBindingsHandle>(hgi->CreateResourceBindings(resourceDesc));
  }

  HgiComputePipelineSharedPtr _CreatePipeline(Hgi *hgi,
                                              uint32_t constantValuesSize,
                                              HgiShaderProgramHandle const &program)
  {
    HgiComputePipelineDesc desc;
    desc.debugName = "SmoothNormals";
    desc.shaderProgram = program;
    desc.shaderConstantsDesc.byteSize = constantValuesSize;
    return std::make_shared<HgiComputePipelineHandle>(hgi->CreateComputePipeline(desc));
  }

}  // Anonymous namespace

HdPh_SmoothNormalsComputationGPU::HdPh_SmoothNormalsComputationGPU(
  Hd_VertexAdjacency const *adjacency,
  TfToken const &srcName,
  TfToken const &dstName,
  HdType srcDataType,
  bool packed)
  : _adjacency(adjacency),
    _srcName(srcName),
    _dstName(dstName),
    _srcDataType(srcDataType)
{
  if (srcDataType != HdTypeFloatVec3 && srcDataType != HdTypeDoubleVec3) {
    TF_CODING_ERROR("Unsupported points type %s for computing smooth normals",
                    TfEnum::GetName(srcDataType).c_str());
    _srcDataType = HdTypeInvalid;
  }
  _dstDataType = packed ? HdTypeInt32_2_10_10_10_REV : _srcDataType;
}

void HdPh_SmoothNormalsComputationGPU::Execute(HdBufferArrayRangeSharedPtr const &range_,
                                               HdResourceRegistry *resourceRegistry)
{
  HD_TRACE_FUNCTION();
  HF_MALLOC_TAG_FUNCTION();

  if (_srcDataType == HdTypeInvalid)
    return;

  TF_VERIFY(_adjacency);
  HdBufferArrayRangeSharedPtr const &adjacencyRange_ = _adjacency->GetAdjacencyRange();
  TF_VERIFY(adjacencyRange_);

  HdPhBufferArrayRangeSharedPtr adjacencyRange = std::static_pointer_cast<HdPhBufferArrayRange>(
    adjacencyRange_);

  // select shader by datatype
  TfToken shaderToken;
  if (_srcDataType == HdTypeFloatVec3) {
    if (_dstDataType == HdTypeFloatVec3) {
      shaderToken = HdPhGLSLProgramTokens->smoothNormalsFloatToFloat;
    } else if (_dstDataType == HdTypeInt32_2_10_10_10_REV) {
      shaderToken = HdPhGLSLProgramTokens->smoothNormalsFloatToPacked;
    }
  } else if (_srcDataType == HdTypeDoubleVec3) {
    if (_dstDataType == HdTypeDoubleVec3) {
      shaderToken = HdPhGLSLProgramTokens->smoothNormalsDoubleToDouble;
    } else if (_dstDataType == HdTypeInt32_2_10_10_10_REV) {
      shaderToken = HdPhGLSLProgramTokens->smoothNormalsDoubleToPacked;
    }
  }
  if (!TF_VERIFY(!shaderToken.IsEmpty()))
    return;

  struct Uniform
  {
    int vertexOffset;
    int adjacencyOffset;
    int pointsOffset;
    int pointsStride;
    int normalsOffset;
    int normalsStride;
  } uniform;

  HdPhResourceRegistry *hdPhResourceRegistry = static_cast<HdPhResourceRegistry *>(
    resourceRegistry);
  HdPhGLSLProgramSharedPtr computeProgram = HdPhGLSLProgram::GetComputeProgram(
    shaderToken,
    hdPhResourceRegistry,
    [&](HgiShaderFunctionDesc &computeDesc) {
      computeDesc.debugName = shaderToken.GetString();
      computeDesc.shaderStage = HgiShaderStageCompute;

      TfToken srcType;
      TfToken dstType;
      if (_srcDataType == HdTypeFloatVec3) {
        srcType = HdPhTokens->_float;
      } else {
        srcType = HdPhTokens->_double;
      }

      if (_dstDataType == HdTypeFloatVec3) {
        dstType = HdPhTokens->_float;
      } else if (_dstDataType == HdTypeDoubleVec3) {
        dstType = HdPhTokens->_double;
      } else if (_dstDataType == HdTypeInt32_2_10_10_10_REV) {
        dstType = HdPhTokens->_int;
      }
      HgiShaderFunctionAddBuffer(&computeDesc, "points", srcType);
      HgiShaderFunctionAddBuffer(&computeDesc, "normals", dstType);
      HgiShaderFunctionAddBuffer(&computeDesc, "entry", HdPhTokens->_int);

      static const std::string params[] = {
        "vertexOffset",     // offset in aggregated buffer
        "adjacencyOffset",  // offset in aggregated buffer
        "pointsOffset",     // interleave offset
        "pointsStride",     // interleave stride
        "normalsOffset",    // interleave offset
        "normalsStride",    // interleave stride
      };
      static_assert((sizeof(Uniform) / sizeof(int)) == (sizeof(params) / sizeof(params[0])), "");
      for (std::string const &param : params) {
        HgiShaderFunctionAddConstantParam(&computeDesc, param, HdPhTokens->_int);
      }
      HgiShaderFunctionAddStageInput(&computeDesc,
                                     "hd_GlobalInvocationID",
                                     "uvec3",
                                     HgiShaderKeywordTokens->hdGlobalInvocationID);
    });

  if (!computeProgram)
    return;

  HdPhBufferArrayRangeSharedPtr range = std::static_pointer_cast<HdPhBufferArrayRange>(range_);

  // buffer resources for GPU computation
  HdPhBufferResourceSharedPtr points = range->GetResource(_srcName);
  HdPhBufferResourceSharedPtr normals = range->GetResource(_dstName);
  HdPhBufferResourceSharedPtr adjacency = adjacencyRange->GetResource();

  // prepare uniform buffer for GPU computation
  // coherent vertex offset in aggregated buffer array
  uniform.vertexOffset = range->GetElementOffset();
  // adjacency offset/stride in aggregated adjacency table
  uniform.adjacencyOffset = adjacencyRange->GetElementOffset();
  // interleaved offset/stride to points
  // note: this code (and the glsl smooth normal compute shader) assumes
  // components in interleaved vertex array are always same data type.
  // i.e. it can't handle an interleaved array which interleaves
  // float/double, float/int etc.
  //
  // The offset and stride values we pass to the shader are in terms
  // of indexes, not bytes, so we must convert the HdBufferResource
  // offset/stride (which are in bytes) to counts of float[]/double[]
  // entries.
  const size_t pointComponentSize = HdDataSizeOfType(
    HdGetComponentType(points->GetTupleType().type));
  uniform.pointsOffset = points->GetOffset() / pointComponentSize;
  uniform.pointsStride = points->GetStride() / pointComponentSize;
  // interleaved offset/stride to normals
  const size_t normalComponentSize = HdDataSizeOfType(
    HdGetComponentType(normals->GetTupleType().type));
  uniform.normalsOffset = normals->GetOffset() / normalComponentSize;
  uniform.normalsStride = normals->GetStride() / normalComponentSize;

  // The number of points is based off the size of the output,
  // However, the number of points in the adjacency table
  // is computed based off the largest vertex indexed from
  // to topology (aka topology->ComputeNumPoints).
  //
  // Therefore, we need to clamp the number of points
  // to the number of entries in the adjancency table.
  int numDestPoints = range->GetNumElements();
  int numSrcPoints = _adjacency->GetNumPoints();

  int numPoints = std::min(numSrcPoints, numDestPoints);

  Hgi *hgi = hdPhResourceRegistry->GetHgi();

  // Generate hash for resource bindings and pipeline.
  // XXX Needs fingerprint hash to avoid collisions
  uint64_t rbHash = (uint64_t)TfHash::Combine(points->GetHandle().Get(),
                                              normals->GetHandle().Get(),
                                              adjacency->GetHandle().Get());

  uint64_t pHash = (uint64_t)TfHash::Combine(computeProgram->GetProgram().Get(), sizeof(uniform));

  // Get or add resource bindings in registry.
  HdInstance<HgiResourceBindingsSharedPtr> resourceBindingsInstance =
    hdPhResourceRegistry->RegisterResourceBindings(rbHash);
  if (resourceBindingsInstance.IsFirstInstance()) {
    HgiResourceBindingsSharedPtr rb = _CreateResourceBindings(hgi,
                                                              points->GetHandle(),
                                                              normals->GetHandle(),
                                                              adjacency->GetHandle());
    resourceBindingsInstance.SetValue(rb);
  }

  HgiResourceBindingsSharedPtr const &resourceBindindsPtr = resourceBindingsInstance.GetValue();
  HgiResourceBindingsHandle resourceBindings = *resourceBindindsPtr.get();

  // Get or add pipeline in registry.
  HdInstance<HgiComputePipelineSharedPtr> computePipelineInstance =
    hdPhResourceRegistry->RegisterComputePipeline(pHash);
  if (computePipelineInstance.IsFirstInstance()) {
    HgiComputePipelineSharedPtr pipe = _CreatePipeline(hgi,
                                                       sizeof(uniform),
                                                       computeProgram->GetProgram());
    computePipelineInstance.SetValue(pipe);
  }

  HgiComputePipelineSharedPtr const &pipelinePtr = computePipelineInstance.GetValue();
  HgiComputePipelineHandle pipeline = *pipelinePtr.get();

  HgiComputeCmds *computeCmds = hdPhResourceRegistry->GetGlobalComputeCmds();
  computeCmds->PushDebugGroup("Smooth Normals Cmds");
  computeCmds->BindResources(resourceBindings);
  computeCmds->BindPipeline(pipeline);

  // transfer uniform buffer
  computeCmds->SetConstantValues(pipeline, BufferBinding_Uniforms, sizeof(uniform), &uniform);

  // dispatch compute kernel
  computeCmds->Dispatch(numPoints, 1);

  // submit the work
  computeCmds->PopDebugGroup();
}

void HdPh_SmoothNormalsComputationGPU::GetBufferSpecs(HdBufferSpecVector *specs) const
{
  specs->emplace_back(_dstName, HdTupleType{_dstDataType, 1});
}

WABI_NAMESPACE_END
