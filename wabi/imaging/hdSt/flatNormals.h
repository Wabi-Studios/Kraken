//
// Copyright 2018 Pixar
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
#ifndef WABI_IMAGING_HD_ST_FLAT_NORMALS_H
#define WABI_IMAGING_HD_ST_FLAT_NORMALS_H

#include "wabi/wabi.h"
#include "wabi/imaging/hdSt/api.h"
#include "wabi/imaging/hd/bufferSource.h"
#include "wabi/imaging/hd/computation.h"

#include "wabi/base/tf/token.h"

WABI_NAMESPACE_BEGIN


class Hd_FaceCoords;

/// flat normal computation GPU
///
///
class HdSt_FlatNormalsComputationGPU : public HdComputation {
public:

    /// Constructor
    HDST_API
    HdSt_FlatNormalsComputationGPU(HdBufferArrayRangeSharedPtr const
                                       &topologyRange,
                                   HdBufferArrayRangeSharedPtr const
                                       &vertexRange,
                                   int numFaces,
                                   TfToken const &srcName,
                                   TfToken const &dstName,
                                   HdType srcDataType,
                                   bool packed);

    HDST_API
    virtual void GetBufferSpecs(HdBufferSpecVector *specs) const override;
    HDST_API
    virtual void Execute(HdBufferArrayRangeSharedPtr const &range,
                         HdResourceRegistry *resourceRegistry) override;

    virtual int GetNumOutputElements() const override;

private:
    HdBufferArrayRangeSharedPtr const _topologyRange;
    HdBufferArrayRangeSharedPtr const _vertexRange;
    int _numFaces;
    TfToken _srcName;
    TfToken _dstName;
    HdType _srcDataType;
    HdType _dstDataType;
};


WABI_NAMESPACE_END

#endif // WABI_IMAGING_HD_ST_FLAT_NORMALS_H
