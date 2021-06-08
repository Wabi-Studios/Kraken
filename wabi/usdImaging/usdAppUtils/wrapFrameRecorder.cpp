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
#include "wabi/usdImaging/usdAppUtils/frameRecorder.h"
#include "wabi/wabi.h"

#include <boost/python.hpp>
#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/scope.hpp>

using namespace boost::python;

WABI_NAMESPACE_USING

void wrapFrameRecorder()
{
  using This = UsdAppUtilsFrameRecorder;

  scope s = class_<This, boost::noncopyable>("FrameRecorder")
                .def(init<>())
                .def("GetCurrentRendererId", &This::GetCurrentRendererId)
                .def("SetRendererPlugin", &This::SetRendererPlugin)
                .def("SetImageWidth", &This::SetImageWidth)
                .def("SetComplexity", &This::SetComplexity)
                .def("SetColorCorrectionMode", &This::SetColorCorrectionMode)
                .def("SetIncludedPurposes", &This::SetIncludedPurposes, (arg("purposes")))
                .def("Record",
                     &This::Record,
                     (arg("stage"), arg("usdCamera"), arg("timeCode"), arg("outputImagePath")));
}
