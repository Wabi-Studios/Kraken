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

#include "wabi/imaging/hd/volumeFieldBindingSchema.h"
#include "wabi/imaging/hd/retainedDataSource.h"

#include "wabi/base/trace/trace.h"


WABI_NAMESPACE_BEGIN

TF_DEFINE_PUBLIC_TOKENS(HdVolumeFieldBindingSchemaTokens,
    HDVOLUMEFIELDBINDING_SCHEMA_TOKENS);


HdPathDataSourceHandle HdVolumeFieldBindingSchema::GetVolumeFieldBinding(TfToken const &name)
{
  return _GetTypedDataSource<HdPathDataSource>(name);
}


/*static*/
HdContainerDataSourceHandle HdVolumeFieldBindingSchema::BuildRetained(
  size_t count,
  TfToken *names,
  HdDataSourceBaseHandle *values)
{
  return HdRetainedContainerDataSource::New(count, names, values);
}

/*static*/
HdVolumeFieldBindingSchema HdVolumeFieldBindingSchema::GetFromParent(
  const HdContainerDataSourceHandle &fromParentContainer)
{
  return HdVolumeFieldBindingSchema(fromParentContainer ?
                                      HdContainerDataSource::Cast(fromParentContainer->Get(
                                        HdVolumeFieldBindingSchemaTokens->volumeFieldBinding)) :
                                      nullptr);
}

/*static*/
const HdDataSourceLocator &HdVolumeFieldBindingSchema::GetDefaultLocator()
{
  static const HdDataSourceLocator locator(HdVolumeFieldBindingSchemaTokens->volumeFieldBinding);
  return locator;
}

WABI_NAMESPACE_END