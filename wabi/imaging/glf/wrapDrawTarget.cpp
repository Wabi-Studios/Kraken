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
#include "wabi/imaging/glf/drawTarget.h"

#include "wabi/base/tf/makePyConstructor.h"
#include "wabi/base/tf/pyPtrHelpers.h"
#include "wabi/base/tf/pyResultConversions.h"
#include "wabi/base/tf/pyEnum.h"

#include <boost/python/bases.hpp>
#include <boost/python/class.hpp>
#include <boost/python/overloads.hpp>

using namespace boost::python;

WABI_NAMESPACE_USING

namespace
{

  static GlfDrawTargetRefPtr _NewDrawTarget(GfVec2i const &size)
  {
    return GlfDrawTarget::New(size);
  }

  static GlfDrawTargetRefPtr _NewDrawTarget2(unsigned int width, unsigned int height)
  {
    return GlfDrawTarget::New(GfVec2i(width, height));
  }

}  // anonymous namespace

void wrapDrawTarget()
{
  typedef GlfDrawTarget This;
  typedef GlfDrawTargetPtr ThisPtr;

  class_<This, ThisPtr, boost::noncopyable>("DrawTarget", no_init)
    .def(TfPyRefAndWeakPtr())
    .def("__init__", TfMakePyConstructor(&_NewDrawTarget))
    .def("__init__", TfMakePyConstructor(&_NewDrawTarget2))
    .def("AddAttachment", &This::AddAttachment)
    .def("Bind", &This::Bind)
    .def("Unbind", &This::Unbind)
    .def("WriteToFile",
         &This::WriteToFile,
         (arg("attachment"),
          arg("filename"),
          arg("viewMatrix") = GfMatrix4d(1),
          arg("projectionMatrix") = GfMatrix4d(1)))

    ;
}

TF_REFPTR_CONST_VOLATILE_GET(GlfDrawTarget)
