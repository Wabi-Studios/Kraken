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

#include "wabi/usd/pcp/mapFunction.h"
#include "wabi/usd/pcp/node.h"
#include "wabi/usd/pcp/pathTranslation.h"
#include "wabi/usd/sdf/path.h"
#include "wabi/wabi.h"

#include <boost/python.hpp>

using namespace boost::python;

WABI_NAMESPACE_USING

namespace {

static SdfPath TranslatePathFromNodeToRoot(const PcpNodeRef &sourceNode, const SdfPath &pathInNodeNamespace)
{
  return PcpTranslatePathFromNodeToRoot(sourceNode, pathInNodeNamespace);
}

static SdfPath TranslatePathFromRootToNode(const PcpNodeRef &destNode, const SdfPath &pathInRootNamespace)
{
  return PcpTranslatePathFromRootToNode(destNode, pathInRootNamespace);
}

}  // anonymous namespace

void wrapPathTranslation()
{
  def("TranslatePathFromNodeToRoot",
      TranslatePathFromNodeToRoot,
      arg("sourceNode"),
      arg("pathInNodeNamespace"));

  def(
    "TranslatePathFromRootToNode", TranslatePathFromRootToNode, arg("destNode"), arg("pathInRootNamespace"));
}
