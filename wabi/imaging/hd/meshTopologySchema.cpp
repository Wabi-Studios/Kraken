//
// Copyright 2021 Pixar
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
////////////////////////////////////////////////////////////////////////
// This file is generated by a script.  Do not edit directly.  Edit the
// schema.template.cpp file to make changes.

#include "wabi/imaging/hd/meshTopologySchema.h"
#include "wabi/imaging/hd/retainedDataSource.h"
#include "wabi/imaging/hd/meshSchema.h"

#include "wabi/base/trace/trace.h"


WABI_NAMESPACE_BEGIN

TF_DEFINE_PUBLIC_TOKENS(HdMeshTopologySchemaTokens,
    HDMESHTOPOLOGY_SCHEMA_TOKENS);


HdIntArrayDataSourceHandle HdMeshTopologySchema::GetFaceVertexCounts()
{
  return _GetTypedDataSource<HdIntArrayDataSource>(HdMeshTopologySchemaTokens->faceVertexCounts);
}

HdIntArrayDataSourceHandle HdMeshTopologySchema::GetFaceVertexIndices()
{
  return _GetTypedDataSource<HdIntArrayDataSource>(HdMeshTopologySchemaTokens->faceVertexIndices);
}

HdIntArrayDataSourceHandle HdMeshTopologySchema::GetHoleIndices()
{
  return _GetTypedDataSource<HdIntArrayDataSource>(HdMeshTopologySchemaTokens->holeIndices);
}

HdTokenDataSourceHandle HdMeshTopologySchema::GetOrientation()
{
  return _GetTypedDataSource<HdTokenDataSource>(HdMeshTopologySchemaTokens->orientation);
}

/*static*/
HdContainerDataSourceHandle HdMeshTopologySchema::BuildRetained(
  const HdIntArrayDataSourceHandle &faceVertexCounts,
  const HdIntArrayDataSourceHandle &faceVertexIndices,
  const HdIntArrayDataSourceHandle &holeIndices,
  const HdTokenDataSourceHandle &orientation)
{
  TfToken names[4];
  HdDataSourceBaseHandle values[4];

  size_t count = 0;
  if (faceVertexCounts) {
    names[count] = HdMeshTopologySchemaTokens->faceVertexCounts;
    values[count++] = faceVertexCounts;
  }

  if (faceVertexIndices) {
    names[count] = HdMeshTopologySchemaTokens->faceVertexIndices;
    values[count++] = faceVertexIndices;
  }

  if (holeIndices) {
    names[count] = HdMeshTopologySchemaTokens->holeIndices;
    values[count++] = holeIndices;
  }

  if (orientation) {
    names[count] = HdMeshTopologySchemaTokens->orientation;
    values[count++] = orientation;
  }

  return HdRetainedContainerDataSource::New(count, names, values);
}

/*static*/
HdMeshTopologySchema HdMeshTopologySchema::GetFromParent(
  const HdContainerDataSourceHandle &fromParentContainer)
{
  return HdMeshTopologySchema(
    fromParentContainer ?
      HdContainerDataSource::Cast(fromParentContainer->Get(HdMeshTopologySchemaTokens->topology)) :
      nullptr);
}

/*static*/
const HdDataSourceLocator &HdMeshTopologySchema::GetDefaultLocator()
{
  static const HdDataSourceLocator locator(HdMeshSchemaTokens->mesh,
                                           HdMeshTopologySchemaTokens->topology);
  return locator;
}

/*static*/
HdTokenDataSourceHandle HdMeshTopologySchema::BuildOrientationDataSource(
  const TfToken &orientation)
{
  if (orientation == HdMeshTopologySchemaTokens->leftHanded) {
    static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
      HdRetainedTypedSampledDataSource<TfToken>::New(orientation);
    return ds;
  }
  if (orientation == HdMeshTopologySchemaTokens->rightHanded) {
    static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
      HdRetainedTypedSampledDataSource<TfToken>::New(orientation);
    return ds;
  }

  // fallback for unknown token
  return HdRetainedTypedSampledDataSource<TfToken>::New(orientation);
}

HdMeshTopologySchema::Builder &HdMeshTopologySchema::Builder::SetFaceVertexCounts(
  const HdIntArrayDataSourceHandle &faceVertexCounts)
{
  _faceVertexCounts = faceVertexCounts;
  return *this;
}

HdMeshTopologySchema::Builder &HdMeshTopologySchema::Builder::SetFaceVertexIndices(
  const HdIntArrayDataSourceHandle &faceVertexIndices)
{
  _faceVertexIndices = faceVertexIndices;
  return *this;
}

HdMeshTopologySchema::Builder &HdMeshTopologySchema::Builder::SetHoleIndices(
  const HdIntArrayDataSourceHandle &holeIndices)
{
  _holeIndices = holeIndices;
  return *this;
}

HdMeshTopologySchema::Builder &HdMeshTopologySchema::Builder::SetOrientation(
  const HdTokenDataSourceHandle &orientation)
{
  _orientation = orientation;
  return *this;
}

HdContainerDataSourceHandle HdMeshTopologySchema::Builder::Build()
{
  return HdMeshTopologySchema::BuildRetained(_faceVertexCounts,
                                             _faceVertexIndices,
                                             _holeIndices,
                                             _orientation);
}


WABI_NAMESPACE_END