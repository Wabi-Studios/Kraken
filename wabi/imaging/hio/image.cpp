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
#include "wabi/imaging/hio/image.h"
#include "wabi/imaging/hio/imageRegistry.h"

#include "wabi/base/tf/type.h"

WABI_NAMESPACE_BEGIN

TF_REGISTRY_FUNCTION(TfType)
{
  TfType::Define<HioImage>();
}

HioImage::~HioImage() = default;

/* static */
bool HioImage::IsSupportedImageFile(std::string const &filename)
{
  HioImageRegistry &registry = HioImageRegistry::GetInstance();
  return registry.IsSupportedImageFile(filename);
}

/* static */
HioImageSharedPtr HioImage::OpenForReading(std::string const &filename,
                                           int subimage,
                                           int mip,
                                           SourceColorSpace sourceColorSpace,
                                           bool suppressErrors)
{
  HioImageRegistry &registry = HioImageRegistry::GetInstance();

  HioImageSharedPtr image = registry._ConstructImage(filename);
  if (!image ||
      !image->_OpenForReading(filename, subimage, mip, sourceColorSpace, suppressErrors)) {
    return HioImageSharedPtr();
  }

  return image;
}

/* static */
HioImageSharedPtr HioImage::OpenForWriting(std::string const &filename)
{
  HioImageRegistry &registry = HioImageRegistry::GetInstance();

  HioImageSharedPtr image = registry._ConstructImage(filename);
  if (!image || !image->_OpenForWriting(filename)) {
    return HioImageSharedPtr();
  }

  return image;
}

WABI_NAMESPACE_END
