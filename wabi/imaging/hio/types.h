//
// Copyright 2020 Pixar
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
#ifndef WABI_IMAGING_HIO_TYPES_H
#define WABI_IMAGING_HIO_TYPES_H

#include "wabi/wabi.h"
#include "wabi/imaging/hio/api.h"
#include <stdlib.h>
#include <cinttypes>

WABI_NAMESPACE_BEGIN

class GfVec3i;

/// \enum HioFormat
///
/// HioFormat describes the memory format of image buffers used in Hio.
///
/// For reference, see:
///  https://www.khronos.org/registry/vulkan/specs/1.1/html/vkspec.html#VkFormat
enum HioFormat
{
  HioFormatInvalid = -1,

  // UNorm8 - a 1-byte value representing a float between 0 and 1.
  // float value = (unorm / 255.0f);
  HioFormatUNorm8 = 0,
  HioFormatUNorm8Vec2,
  HioFormatUNorm8Vec3,
  HioFormatUNorm8Vec4,

  // SNorm8 - a 1-byte value representing a float between -1 and 1.
  // float value = max(snorm / 127.0f, -1.0f);
  HioFormatSNorm8,
  HioFormatSNorm8Vec2,
  HioFormatSNorm8Vec3,
  HioFormatSNorm8Vec4,

  // Float16 - a 2-byte IEEE half-precision float.
  HioFormatFloat16,
  HioFormatFloat16Vec2,
  HioFormatFloat16Vec3,
  HioFormatFloat16Vec4,

  // Float32 - a 4-byte IEEE float.
  HioFormatFloat32,
  HioFormatFloat32Vec2,
  HioFormatFloat32Vec3,
  HioFormatFloat32Vec4,

  // Double64 - a 8-byte IEEE double.
  HioFormatDouble64,
  HioFormatDouble64Vec2,
  HioFormatDouble64Vec3,
  HioFormatDouble64Vec4,

  // UInt16 - a 2-byte unsigned short integer.
  HioFormatUInt16,
  HioFormatUInt16Vec2,
  HioFormatUInt16Vec3,
  HioFormatUInt16Vec4,

  // Int16 - a 2-byte signed short integer.
  HioFormatInt16,
  HioFormatInt16Vec2,
  HioFormatInt16Vec3,
  HioFormatInt16Vec4,

  // UInt32 - a 4-byte unsigned integer.
  HioFormatUInt32,
  HioFormatUInt32Vec2,
  HioFormatUInt32Vec3,
  HioFormatUInt32Vec4,

  // Int32 - a 4-byte signed integer.
  HioFormatInt32,
  HioFormatInt32Vec2,
  HioFormatInt32Vec3,
  HioFormatInt32Vec4,

  // UNorm8 SRGB - a 1-byte value representing a float between 0 and 1.
  HioFormatUNorm8srgb,
  HioFormatUNorm8Vec2srgb,
  HioFormatUNorm8Vec3srgb,
  HioFormatUNorm8Vec4srgb,

  // BPTC compressed. 3-component, 4x4 blocks, signed floating-point
  HioFormatBC6FloatVec3,

  // BPTC compressed. 3-component, 4x4 blocks, unsigned floating-point
  HioFormatBC6UFloatVec3,

  // BPTC compressed. 4-component, 4x4 blocks, unsigned byte.
  // Representing a float between 0 and 1.
  HioFormatBC7UNorm8Vec4,

  // BPTC compressed. 4-component, 4x4 blocks, unsigned byte, sRGB.
  // Representing a float between 0 and 1.
  HioFormatBC7UNorm8Vec4srgb,

  // S3TC/DXT compressed. 4-component, 4x4 blocks, unsigned byte
  // Representing a float between 0 and 1.
  HioFormatBC1UNorm8Vec4,

  // S3TC/DXT compressed. 4-component, 4x4 blocks, unsigned byte
  // Representing a float between 0 and 1.
  HioFormatBC3UNorm8Vec4,

  HioFormatCount
};

/// \enum HioAddressDimension
///
/// Available texture sampling dimensions.
///
enum HioAddressDimension
{
  HioAddressDimensionU,
  HioAddressDimensionV,
  HioAddressDimensionW
};

/// \enum HioAddressMode
///
/// Various modes used during sampling of a texture.
///
enum HioAddressMode
{
  HioAddressModeClampToEdge = 0,
  HioAddressModeMirrorClampToEdge,
  HioAddressModeRepeat,
  HioAddressModeMirrorRepeat,
  HioAddressModeClampToBorderColor
};

/// \enum HioColorChannelType
///
/// Various color channel representation formats.
///
enum HioType
{
  HioTypeUnsignedByte,
  HioTypeUnsignedByteSRGB,
  HioTypeSignedByte,
  HioTypeUnsignedShort,
  HioTypeSignedShort,
  HioTypeUnsignedInt,
  HioTypeInt,
  HioTypeHalfFloat,
  HioTypeFloat,
  HioTypeDouble,

  HioTypeCount
};

/// Returns the HioFormat of containing nChannels of HioType type.
HIO_API
HioFormat HioGetFormat(uint32_t nchannels, HioType type, bool isSRGB);

/// Return the HioType corresponding to the given HioFormat
HIO_API
HioType HioGetHioType(HioFormat);

/// Return the count of components (channels) in the given HioFormat.
HIO_API
int HioGetComponentCount(HioFormat format);

/// Return the size in bytes for a component (channel) in the given HioFormat.
HIO_API
size_t HioGetDataSizeOfType(HioFormat hioFormat);

/// Return the size in bytes for a component (channel) in the given HioType.
HIO_API
size_t HioGetDataSizeOfType(HioType type);

/// Returns the size of bytes per pixel for the given HioFormat
HIO_API
size_t HioGetDataSizeOfFormat(HioFormat format,
                              size_t *blockWidth = nullptr,
                              size_t *blockHeight = nullptr);

/// Return if the given format is compressed.
HIO_API
bool HioIsCompressed(HioFormat format);

/// Calculate the byte size of texture. If compressed, takes block size
/// into account.
HIO_API
size_t HioGetDataSize(const HioFormat hioFormat, const GfVec3i &dimensions);

WABI_NAMESPACE_END

#endif
