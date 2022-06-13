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

#ifndef WABI_USD_IMAGING_USD_IMAGING_DATA_SOURCE_VOLUME_H
#define WABI_USD_IMAGING_USD_IMAGING_DATA_SOURCE_VOLUME_H

#include "wabi/usdImaging/usdImaging/dataSourceGprim.h"
#include "wabi/usdImaging/usdImaging/dataSourceStageGlobals.h"

#include "wabi/usd/usdVol/volume.h"


WABI_NAMESPACE_BEGIN

/// \class UsdImagingDataSourceVolumeFieldBindings
///
/// A container data source representing volume field binding information.
///
class UsdImagingDataSourceVolumeFieldBindings : public HdContainerDataSource
{
public:
    HD_DECLARE_DATASOURCE(UsdImagingDataSourceVolumeFieldBindings);

    bool Has(const TfToken & name) override;
    TfTokenVector GetNames() override;
    HdDataSourceBaseHandle Get(const TfToken & name) override;

private:

    // Private constructor, use static New() instead.
    UsdImagingDataSourceVolumeFieldBindings(
            UsdVolVolume usdVolume,
            const UsdImagingDataSourceStageGlobals &stageGlobals);


private:
    UsdVolVolume _usdVolume;
    const UsdImagingDataSourceStageGlobals &_stageGlobals;
};

HD_DECLARE_DATASOURCE_HANDLES(UsdImagingDataSourceVolumeFieldBindings);

// ----------------------------------------------------------------------------

/// \class UsdImagingDataSourceVolumePrim
///
/// A prim data source representing a UsdVolVolume prim. 
///
class UsdImagingDataSourceVolumePrim : public UsdImagingDataSourceGprim
{
public:
    HD_DECLARE_DATASOURCE(UsdImagingDataSourceVolumePrim);

    bool Has(const TfToken &name) override;
    TfTokenVector GetNames() override;
    HdDataSourceBaseHandle Get(const TfToken &name) override;

private:
    // Private constructor, use static New() instead.
    UsdImagingDataSourceVolumePrim(
        const SdfPath &sceneIndexPath,
        UsdPrim usdPrim,
        const UsdImagingDataSourceStageGlobals &stageGlobals);
};

HD_DECLARE_DATASOURCE_HANDLES(UsdImagingDataSourceVolumePrim);

WABI_NAMESPACE_END

#endif // WABI_USD_IMAGING_USD_IMAGING_DATA_SOURCE_VOLUME_H
