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

#include "tokens.h"

WABI_NAMESPACE_BEGIN

HdCyclesTokensType::HdCyclesTokensType()
  : cpu("cpu", TfToken::Immortal),
    gpu("gpu", TfToken::Immortal),
    svm("svm", TfToken::Immortal),
    osl("osl", TfToken::Immortal),
    hilbert_spiral("hilbert_spiral", TfToken::Immortal),
    right_to_left("right_to_left", TfToken::Immortal),
    left_to_right("left_to_right", TfToken::Immortal),
    top_to_bottom("top_to_bottom", TfToken::Immortal),
    bottom_to_top("bottom_to_top", TfToken::Immortal),
    bvh_dynamic("bvh_dynamic", TfToken::Immortal),
    bvh_static("bvh_static", TfToken::Immortal),
    none("none", TfToken::Immortal),
    optix("optix", TfToken::Immortal),
    openimagedenoise("openimagedenoise", TfToken::Immortal),
    rgb_albedo_normal("rgb_albedo_normal", TfToken::Immortal),
    rgb("rgb", TfToken::Immortal),
    rgb_albedo("rgb_albedo", TfToken::Immortal),
    branched_path("branched_path", TfToken::Immortal),
    path("path", TfToken::Immortal),
    sobol("sobol", TfToken::Immortal),
    cmj("cmj", TfToken::Immortal),
    pmj("pmj", TfToken::Immortal),
    box("box", TfToken::Immortal),
    gaussian("gaussian", TfToken::Immortal),
    blackman_harris("blackman_harris", TfToken::Immortal),
    center("center", TfToken::Immortal),
    start("start", TfToken::Immortal),
    end("end", TfToken::Immortal),
    top("top", TfToken::Immortal),
    left("left", TfToken::Immortal),
    right("right", TfToken::Immortal),
    equirectangular("equirectangular", TfToken::Immortal),
    fisheye_equidistant("fisheye_equidistant", TfToken::Immortal),
    fisheye_equisolid("fisheye_equisolid", TfToken::Immortal),
    mirrorball("mirrorball", TfToken::Immortal),
    displacement_bump("displacement_bump", TfToken::Immortal),
    displacement_true("displacement_true", TfToken::Immortal),
    displacement_both("displacement_both", TfToken::Immortal),
    volume_sampling_distance("volume_sampling_distance", TfToken::Immortal),
    volume_sampling_equiangular("volume_sampling_equiangular", TfToken::Immortal),
    volume_sampling_multiple_importance("volume_sampling_multiple_importance", TfToken::Immortal),
    volume_interpolation_linear("volume_interpolation_linear", TfToken::Immortal),
    volume_interpolation_cubic("volume_interpolation_cubic", TfToken::Immortal),
    ribbon("ribbon", TfToken::Immortal),
    thick("thick", TfToken::Immortal),
    disc("disc", TfToken::Immortal),
    disc_oriented("disc_oriented", TfToken::Immortal),
    sphere("sphere", TfToken::Immortal),
    combined("combined", TfToken::Immortal),
    ambient_occlusion("ambient_occlusion", TfToken::Immortal),
    shadow("shadow", TfToken::Immortal),
    normal("normal", TfToken::Immortal),
    uv("uv", TfToken::Immortal),
    roughness("roughness", TfToken::Immortal),
    emit("emit", TfToken::Immortal),
    environment("environment", TfToken::Immortal),
    diffuse("diffuse", TfToken::Immortal),
    glossy("glossy", TfToken::Immortal),
    transmission("transmission", TfToken::Immortal),
    diffuse_color("diffuse_color", TfToken::Immortal),
    glossy_color("glossy_color", TfToken::Immortal),
    transmission_color("transmission_color", TfToken::Immortal),
    primvarsCyclesObjectMblur("primvars:cycles:object:mblur", TfToken::Immortal),
    primvarsCyclesObjectTransformSamples("primvars:cycles:object:transform:samples",
                                         TfToken::Immortal),
    primvarsCyclesObjectDeformSamples("primvars:cycles:object:deform:samples", TfToken::Immortal),
    primvarsCyclesObjectMblurVolumeVelScale("primvars:cycles:object:mblur:volume_vel_scale",
                                            TfToken::Immortal),
    primvarsCyclesObjectPassId("primvars:cycles:object:pass_id", TfToken::Immortal),
    primvarsCyclesObjectUseHoldout("primvars:cycles:object:use_holdout", TfToken::Immortal),
    primvarsCyclesObjectIsShadowCatcher("primvars:cycles:object:is_shadow_catcher",
                                        TfToken::Immortal),
    primvarsCyclesObjectVisibilityCamera("primvars:cycles:object:visibility:camera",
                                         TfToken::Immortal),
    primvarsCyclesObjectVisibilityDiffuse("primvars:cycles:object:visibility:diffuse",
                                          TfToken::Immortal),
    primvarsCyclesObjectVisibilityGlossy("primvars:cycles:object:visibility:glossy",
                                         TfToken::Immortal),
    primvarsCyclesObjectVisibilityTransmission("primvars:cycles:object:visibility:transmission",
                                               TfToken::Immortal),
    primvarsCyclesObjectVisibilityShadow("primvars:cycles:object:visibility:shadow",
                                         TfToken::Immortal),
    primvarsCyclesObjectVisibilityScatter("primvars:cycles:object:visibility:scatter",
                                          TfToken::Immortal),
    primvarsCyclesObjectAssetName("primvars:cycles:object:asset_name", TfToken::Immortal),
    primvarsCyclesMeshDicingRate("primvars:cycles:mesh:dicing:rate", TfToken::Immortal),
    primvarsCyclesMeshSubdivisionMaxLevel("primvars:cycles:mesh:subdivision_max_level",
                                          TfToken::Immortal),
    primvarsCyclesCurveShape("primvars:cycles:curve:shape", TfToken::Immortal),
    cyclesDevice("cycles:device", TfToken::Immortal),
    cyclesShadingSystem("cycles:shading_system", TfToken::Immortal),
    cyclesProgressive("cycles:progressive", TfToken::Immortal),
    cyclesProgressiveRefine("cycles:progressive_refine", TfToken::Immortal),
    cyclesProgressiveUpdateTimeout("cycles:progressive_update_timeout", TfToken::Immortal),
    cyclesAdaptiveSampling("cycles:adaptive_sampling", TfToken::Immortal),
    cyclesStartResolution("cycles:start_resolution", TfToken::Immortal),
    cyclesPixelSize("cycles:pixel_size", TfToken::Immortal),
    cyclesSamples("cycles:samples", TfToken::Immortal),
    cyclesThreads("cycles:threads", TfToken::Immortal),
    cyclesTileSize("cycles:tile_size", TfToken::Immortal),
    cyclesTileOrder("cycles:tile_order", TfToken::Immortal),
    cyclesExperimental("cycles:experimental", TfToken::Immortal),
    cyclesUseSquareSamples("cycles:use_square_samples", TfToken::Immortal),
    cyclesDicingCamera("cycles:dicing_camera", TfToken::Immortal),
    cyclesUseProfiling("cycles:use_profiling", TfToken::Immortal),
    cyclesUseBvhUnalignedNodes("cycles:use_bgh_unaligned_nodes", TfToken::Immortal),
    cyclesBvhType("cycles:bvh_type", TfToken::Immortal),
    cyclesUseBvhSpatialSplit("cycles:use_bvh_spatial_split", TfToken::Immortal),
    cyclesNumBvhTimeSteps("cycles:num_bvh_time_steps", TfToken::Immortal),
    cyclesDisplayBufferLinear("cycles:display_buffer_linear", TfToken::Immortal),
    cyclesCurveSubdivisions("cycles:curve_subdivisions", TfToken::Immortal),
    cyclesDenoiseUse("cycles:denoise:denoise_use", TfToken::Immortal),
    cyclesDenoiseStorePasses("cycles:denoise:store_passes", TfToken::Immortal),
    cyclesDenoiseType("cycles:denoise:type", TfToken::Immortal),
    cyclesDenoiseStartSample("cycles:denoise:start_samples", TfToken::Immortal),
    cyclesDenoiseInputPasses("cycles:denoise:input_passes", TfToken::Immortal),
    cyclesIntegratorMethod("cycles:integrator:method", TfToken::Immortal),
    cyclesIntegratorSamplingMethod("cycles:integrator:sampling_method", TfToken::Immortal),
    cyclesIntegratorSeed("cycles:integrator:seed", TfToken::Immortal),
    cyclesIntegratorMinBounce("cycles:integrator:min_bounce", TfToken::Immortal),
    cyclesIntegratorMaxBounce("cycles:integrator:max_bounce", TfToken::Immortal),
    cyclesIntegratorMaxDiffuseBounce("cycles:integrator:diffuse_bounce", TfToken::Immortal),
    cyclesIntegratorMaxGlossyBounce("cycles:integrator:glossy_bounce", TfToken::Immortal),
    cyclesIntegratorMaxTransmissionBounce("cycles:integrator:transmission_bounce",
                                          TfToken::Immortal),
    cyclesIntegratorMaxVolumeBounce("cycles:integrator:volume_bounce", TfToken::Immortal),
    cyclesIntegratorTransparentMinBounce("cycles:integrator:transparent_min_bounce",
                                         TfToken::Immortal),
    cyclesIntegratorTransparentMaxBounce("cycles:integrator:transparent_max_bounce",
                                         TfToken::Immortal),
    cyclesIntegratorAoBounces("cycles:integrator:ao_bounces", TfToken::Immortal),
    cyclesIntegratorVolumeMaxSteps("cycles:integrator:volume_max_steps", TfToken::Immortal),
    cyclesIntegratorVolumeStepSize("cycles:integrator:volume_step_size", TfToken::Immortal),
    cyclesIntegratorAaSamples("cycles:integrator:aa_samples", TfToken::Immortal),
    cyclesIntegratorDiffuseSamples("cycles:integrator:diffuse_samples", TfToken::Immortal),
    cyclesIntegratorGlossySamples("cycles:integrator:glossy_samples", TfToken::Immortal),
    cyclesIntegratorTransmissionSamples("cycles:integrator:transmission_samples",
                                        TfToken::Immortal),
    cyclesIntegratorAoSamples("cycles:integrator:ao_samples", TfToken::Immortal),
    cyclesIntegratorMeshLightSamples("cycles:integrator:mesh_light_samples", TfToken::Immortal),
    cyclesIntegratorSubsurfaceSamples("cycles:integrator:subsurface_samples", TfToken::Immortal),
    cyclesIntegratorVolumeSamples("cycles:integrator:volume_samples", TfToken::Immortal),
    cyclesIntegratorStartSample("cycles:integrator:start_sample", TfToken::Immortal),
    cyclesIntegratorCausticsReflective("cycles:integrator:caustics_reflective", TfToken::Immortal),
    cyclesIntegratorCausticsRefractive("cycles:integrator:caustics_refractive", TfToken::Immortal),
    cyclesIntegratorFilterGlossy("cycles:integrator:filter_glossy", TfToken::Immortal),
    cyclesIntegratorSampleClampDirect("cycles:integrator:sample_clamp_direct", TfToken::Immortal),
    cyclesIntegratorSampleClampIndirect("cycles:integrator:sample_clamp_indirect",
                                        TfToken::Immortal),
    cyclesIntegratorMotionBlur("cycles:integrator:motion_blur", TfToken::Immortal),
    cyclesIntegratorSampleAllLightsDirect("cycles:integrator:sample_all_lights_direct",
                                          TfToken::Immortal),
    cyclesIntegratorSampleAllLightsIndirect("cycles:integrator:sample_all_lights_indirect",
                                            TfToken::Immortal),
    cyclesIntegratorLightSamplingThreshold("cycles:integrator:light_sampling_threshold",
                                           TfToken::Immortal),
    cyclesIntegratorAdaptiveMinSamples("cycles:integrator:adaptive_min_samples",
                                       TfToken::Immortal),
    cyclesIntegratorAdaptiveThreshold("cycles:integrator:adaptive_threshold", TfToken::Immortal),
    cyclesFilmExposure("cycles:film:exposure", TfToken::Immortal),
    cyclesFilmPassAlphaThreshold("cycles:film:pass_alpha_threshold", TfToken::Immortal),
    cyclesFilmFilterType("cycles:film:filter_type", TfToken::Immortal),
    cyclesFilmFilterWidth("cycles:film:filter_width", TfToken::Immortal),
    cyclesFilmMistStart("cycles:film:mist_start", TfToken::Immortal),
    cyclesFilmMistDepth("cycles:film:mist_depth", TfToken::Immortal),
    cyclesFilmMistFalloff("cycles:film:mist_falloff", TfToken::Immortal),
    cyclesFilmUseLightVisibility("cycles:film:use_light_visibility", TfToken::Immortal),
    cyclesFilmUseAdaptiveSampling("cycles:film:use_adaptive_sampling", TfToken::Immortal),
    cyclesFilmDenoisingDataPass("cycles:film:denoising_data_pass", TfToken::Immortal),
    cyclesFilmDenoisingCleanPass("cycles:film:denoising_clean_pass", TfToken::Immortal),
    cyclesFilmDenoisingPrefilteredPass("cycles:film:denoising_prefiltered_pass",
                                       TfToken::Immortal),
    cyclesFilmDenoisingFlags("cycles:film:denoising_flags", TfToken::Immortal),
    cyclesFilmCryptomatteDepth("cycles:film:cryptomatte_depth", TfToken::Immortal),
    cyclesTextureUseCache("cycles:texture:use_cache", TfToken::Immortal),
    cyclesTextureCacheSize("cycles:texture:cache_size", TfToken::Immortal),
    cyclesTextureTileSize("cycles:texture:tile_size", TfToken::Immortal),
    cyclesTextureDiffuseBlur("cycles:texture:diffuse_blur", TfToken::Immortal),
    cyclesTextureGlossyBlur("cycles:texture:glossy_blur", TfToken::Immortal),
    cyclesTextureAutoConvert("cycles:texture:auto_convert", TfToken::Immortal),
    cyclesTextureAcceptUnmipped("cycles:texture:accept_unmipped", TfToken::Immortal),
    cyclesTextureAcceptUntiled("cycles:texture:accept_untiled", TfToken::Immortal),
    cyclesTextureAutoTile("cycles:texture:auto_tile", TfToken::Immortal),
    cyclesTextureAutoMip("cycles:texture:auto_mip", TfToken::Immortal),
    cyclesTextureUseCustomPath("cycles:texture:use_custom_path", TfToken::Immortal),
    cyclesTextureMaxSize("cycles:texture:max_size", TfToken::Immortal),
    cyclesLightUseMis("cycles:light:use_mis", TfToken::Immortal),
    cyclesLightUseDiffuse("cycles:light:use_diffuse", TfToken::Immortal),
    cyclesLightUseGlossy("cycles:light:use_glossy", TfToken::Immortal),
    cyclesLightUseTransmission("cycles:light:use_transmission", TfToken::Immortal),
    cyclesLightUseScatter("cycles:light:use_scatter", TfToken::Immortal),
    cyclesLightIsPortal("cycles:light:is_portal", TfToken::Immortal),
    cyclesLightSamples("cycles:light:samples", TfToken::Immortal),
    cyclesLightMaxBounces("cycles:light:max_bounces", TfToken::Immortal),
    cyclesLightMapResolution("cycles:light:map_resolution", TfToken::Immortal),
    cyclesCameraMotionPosition("cycles:camera:motion_position", TfToken::Immortal),
    cyclesCameraShutterTime("cycles:camera:shutter_time", TfToken::Immortal),
    cyclesCameraShutterCurve("cycles:camera:shutter_curve", TfToken::Immortal),
    cyclesCameraRollingShutterType("cycles:camera:rolling_shutter_type", TfToken::Immortal),
    cyclesCameraRollingShutterDuration("cycles:camera:rolling_shutter_duration",
                                       TfToken::Immortal),
    cyclesCameraStereoEye("cycles:camera:stereo_eye", TfToken::Immortal),
    cyclesCameraUsePanoramic("cycles:camera:use_panoramic", TfToken::Immortal),
    cyclesCameraPanoramaType("cycles:camera:panorama_type", TfToken::Immortal),
    cyclesCameraBlades("cycles:camera:blades", TfToken::Immortal),
    cyclesCameraBladesRotation("cycles:camera:blades_rotation", TfToken::Immortal),
    cyclesCameraOffscreenDicingScale("cycles:camera:offscreen_dicing_scale", TfToken::Immortal),
    cyclesCameraFisheyeFov("cycles:camera:fisheye_fov", TfToken::Immortal),
    cyclesCameraFisheyeLens("cycles:camera:fisheye_lens", TfToken::Immortal),
    cyclesCameraLatitudeMin("cycles:camera:latitude_min", TfToken::Immortal),
    cyclesCameraLatitudeMax("cycles:camera:latitude_max", TfToken::Immortal),
    cyclesCameraLongitudeMin("cycles:camera:longitude_min", TfToken::Immortal),
    cyclesCameraLongitudeMax("cycles:camera:longitude_max", TfToken::Immortal),
    cyclesCameraUseSphericalStereo("cycles:camera:use_spherical_stereo", TfToken::Immortal),
    cyclesCameraInterocularDistance("cycles:camera:interocular_distance", TfToken::Immortal),
    cyclesCameraConvergenceDistance("cycles:camera:convergence_distance", TfToken::Immortal),
    cyclesCameraUsePoleMerge("cycles:camera:use_pole_merge", TfToken::Immortal),
    cyclesCameraPoleMergeAngleFrom("cycles:camera:pole_merge_angle_from", TfToken::Immortal),
    cyclesCameraPoleMergeAngleTo("cycles:camera:pole_merge_angle_to", TfToken::Immortal),
    cyclesMaterialPassId("cycles:material:pass_id", TfToken::Immortal),
    cyclesMaterialDisplacementMethod("cycles:material:displacement_method", TfToken::Immortal),
    cyclesMaterialUseMis("cycles:material:use_mis", TfToken::Immortal),
    cyclesMaterialUseTransparentShadow("cycles:material:use_transparent_shadow",
                                       TfToken::Immortal),
    cyclesMaterialHeterogeneousVolume("cycles:material:heterogeneous_volume", TfToken::Immortal),
    cyclesMaterialVolumeSamplingMethod("cycles:material:volume_sampling_method",
                                       TfToken::Immortal),
    cyclesMaterialVolumeInterpolationMethod("cycles:material:volume_interpolation_method",
                                            TfToken::Immortal),
    cyclesMaterialVolumeStepRate("cycles:material:volume_step_rate", TfToken::Immortal),
    cyclesBackgroundAoFactor("cycles:background:ao_factor", TfToken::Immortal),
    cyclesBackgroundAoDistance("cycles:background:ao_distance", TfToken::Immortal),
    cyclesBackgroundUseShader("cycles:background:use_shader", TfToken::Immortal),
    cyclesBackgroundUseAo("cycles:background:use_ao", TfToken::Immortal),
    cyclesBackgroundTransparent("cycles:background:transparent", TfToken::Immortal),
    cyclesBackgroundTransparentGlass("cycles:background:transparent_glass", TfToken::Immortal),
    cyclesBackgroundTransparentRoughnessThreshold(
      "cycles:background:transparent_roughness_threshold",
      TfToken::Immortal),
    cyclesBackgroundVolumeStepSize("cycles:background:volume_step_size", TfToken::Immortal),
    cyclesBackgroundVisibilityCamera("cycles:background:visibility_camera", TfToken::Immortal),
    cyclesBackgroundVisibilityDiffuse("cycles:background:visibility_diffuse", TfToken::Immortal),
    cyclesBackgroundVisibilityGlossy("cycles:background:visibility_glossy", TfToken::Immortal),
    cyclesBackgroundVisibilityTransmission("cycles:background:visibility_transmission",
                                           TfToken::Immortal),
    cyclesBackgroundVisibilityScatter("cycles:background:visibility_scatter", TfToken::Immortal),
    cyclesObjectPointStyle("cycles:object:point_style", TfToken::Immortal),
    cyclesObjectPointResolution("cycles:object:point_resolution", TfToken::Immortal),
    cyclesBakeEnable("cycles:bake:enable", TfToken::Immortal),
    cyclesBakeObject("cycles:bake:object", TfToken::Immortal),
    cyclesBakeBakeType("cycles:bake:bake_type", TfToken::Immortal),
    cyclesBakeDirect("cycles:bake:direct", TfToken::Immortal),
    cyclesBakeIndirect("cycles:bake:indirect", TfToken::Immortal),
    cyclesBakeFilterColor("cycles:bake:filter:color", TfToken::Immortal),
    cyclesBakeFilterDiffuse("cycles:bake:filter:diffuse", TfToken::Immortal),
    cyclesBakeFilterGlossy("cycles:bake:filter:glossy", TfToken::Immortal),
    cyclesBakeFilterTransmission("cycles:bake:filter:transmission", TfToken::Immortal),
    cyclesBakeFilterAmbientOcclusion("cycles:bake:filter:ambient_occlusion", TfToken::Immortal),
    cyclesBakeFilterEmission("cycles:bake:filter:emission", TfToken::Immortal),
    cyclesBakeMargin("cycles:bake:margin", TfToken::Immortal),
    allTokens({
      cpu,
      gpu,
      svm,
      osl,
      hilbert_spiral,
      center,
      right_to_left,
      left_to_right,
      top_to_bottom,
      bottom_to_top,
      bvh_dynamic,
      bvh_static,
      none,
      optix,
      openimagedenoise,
      rgb_albedo_normal,
      rgb,
      rgb_albedo,
      branched_path,
      path,
      sobol,
      cmj,
      pmj,
      box,
      gaussian,
      blackman_harris,
      center,
      start,
      end,
      none,
      top,
      left,
      right,
      equirectangular,
      fisheye_equidistant,
      fisheye_equisolid,
      mirrorball,
      displacement_bump,
      displacement_true,
      displacement_both,
      volume_sampling_distance,
      volume_sampling_equiangular,
      volume_sampling_multiple_importance,
      volume_interpolation_linear,
      volume_interpolation_cubic,
      ribbon,
      thick,
      disc,
      disc_oriented,
      sphere,
      combined,
      ambient_occlusion,
      shadow,
      normal,
      uv,
      roughness,
      emit,
      environment,
      diffuse,
      glossy,
      transmission,
      diffuse_color,
      glossy_color,
      transmission_color,
      primvarsCyclesObjectMblur,
      primvarsCyclesObjectTransformSamples,
      primvarsCyclesObjectDeformSamples,
      primvarsCyclesObjectMblurVolumeVelScale,
      primvarsCyclesObjectPassId,
      primvarsCyclesObjectUseHoldout,
      primvarsCyclesObjectIsShadowCatcher,
      primvarsCyclesObjectVisibilityCamera,
      primvarsCyclesObjectVisibilityDiffuse,
      primvarsCyclesObjectVisibilityGlossy,
      primvarsCyclesObjectVisibilityTransmission,
      primvarsCyclesObjectVisibilityShadow,
      primvarsCyclesObjectVisibilityScatter,
      primvarsCyclesObjectAssetName,
      primvarsCyclesMeshDicingRate,
      primvarsCyclesMeshSubdivisionMaxLevel,
      primvarsCyclesCurveShape,
      cyclesDevice,
      cyclesShadingSystem,
      cyclesProgressive,
      cyclesProgressiveRefine,
      cyclesProgressiveUpdateTimeout,
      cyclesAdaptiveSampling,
      cyclesStartResolution,
      cyclesPixelSize,
      cyclesSamples,
      cyclesThreads,
      cyclesTileSize,
      cyclesTileOrder,
      cyclesExperimental,
      cyclesUseSquareSamples,
      cyclesDicingCamera,
      cyclesUseProfiling,
      cyclesUseBvhUnalignedNodes,
      cyclesBvhType,
      cyclesUseBvhSpatialSplit,
      cyclesNumBvhTimeSteps,
      cyclesDisplayBufferLinear,
      cyclesCurveSubdivisions,
      cyclesDenoiseUse,
      cyclesDenoiseStorePasses,
      cyclesDenoiseType,
      cyclesDenoiseStartSample,
      cyclesDenoiseInputPasses,
      cyclesIntegratorMethod,
      cyclesIntegratorSamplingMethod,
      cyclesIntegratorSeed,
      cyclesIntegratorMinBounce,
      cyclesIntegratorMaxBounce,
      cyclesIntegratorMaxDiffuseBounce,
      cyclesIntegratorMaxGlossyBounce,
      cyclesIntegratorMaxTransmissionBounce,
      cyclesIntegratorMaxVolumeBounce,
      cyclesIntegratorTransparentMinBounce,
      cyclesIntegratorTransparentMaxBounce,
      cyclesIntegratorAoBounces,
      cyclesIntegratorVolumeMaxSteps,
      cyclesIntegratorVolumeStepSize,
      cyclesIntegratorAaSamples,
      cyclesIntegratorDiffuseSamples,
      cyclesIntegratorGlossySamples,
      cyclesIntegratorTransmissionSamples,
      cyclesIntegratorAoSamples,
      cyclesIntegratorMeshLightSamples,
      cyclesIntegratorSubsurfaceSamples,
      cyclesIntegratorVolumeSamples,
      cyclesIntegratorStartSample,
      cyclesIntegratorCausticsReflective,
      cyclesIntegratorCausticsRefractive,
      cyclesIntegratorFilterGlossy,
      cyclesIntegratorSampleClampDirect,
      cyclesIntegratorSampleClampIndirect,
      cyclesIntegratorMotionBlur,
      cyclesIntegratorSampleAllLightsDirect,
      cyclesIntegratorSampleAllLightsIndirect,
      cyclesIntegratorLightSamplingThreshold,
      cyclesIntegratorAdaptiveMinSamples,
      cyclesIntegratorAdaptiveThreshold,
      cyclesFilmExposure,
      cyclesFilmPassAlphaThreshold,
      cyclesFilmFilterType,
      cyclesFilmFilterWidth,
      cyclesFilmMistStart,
      cyclesFilmMistDepth,
      cyclesFilmMistFalloff,
      cyclesFilmUseLightVisibility,
      cyclesFilmUseAdaptiveSampling,
      cyclesFilmDenoisingDataPass,
      cyclesFilmDenoisingCleanPass,
      cyclesFilmDenoisingPrefilteredPass,
      cyclesFilmDenoisingFlags,
      cyclesFilmCryptomatteDepth,
      cyclesTextureUseCache,
      cyclesTextureCacheSize,
      cyclesTextureTileSize,
      cyclesTextureDiffuseBlur,
      cyclesTextureGlossyBlur,
      cyclesTextureAutoConvert,
      cyclesTextureAcceptUnmipped,
      cyclesTextureAcceptUntiled,
      cyclesTextureAutoTile,
      cyclesTextureAutoMip,
      cyclesTextureUseCustomPath,
      cyclesTextureMaxSize,
      cyclesLightUseMis,
      cyclesLightUseDiffuse,
      cyclesLightUseGlossy,
      cyclesLightUseTransmission,
      cyclesLightUseScatter,
      cyclesLightIsPortal,
      cyclesLightSamples,
      cyclesLightMaxBounces,
      cyclesLightMapResolution,
      cyclesCameraMotionPosition,
      cyclesCameraShutterTime,
      cyclesCameraShutterCurve,
      cyclesCameraRollingShutterType,
      cyclesCameraRollingShutterDuration,
      cyclesCameraStereoEye,
      cyclesCameraUsePanoramic,
      cyclesCameraPanoramaType,
      cyclesCameraBlades,
      cyclesCameraBladesRotation,
      cyclesCameraOffscreenDicingScale,
      cyclesCameraFisheyeFov,
      cyclesCameraFisheyeLens,
      cyclesCameraLatitudeMin,
      cyclesCameraLatitudeMax,
      cyclesCameraLongitudeMin,
      cyclesCameraLongitudeMax,
      cyclesCameraUseSphericalStereo,
      cyclesCameraInterocularDistance,
      cyclesCameraConvergenceDistance,
      cyclesCameraUsePoleMerge,
      cyclesCameraPoleMergeAngleFrom,
      cyclesCameraPoleMergeAngleTo,
      cyclesMaterialPassId,
      cyclesMaterialDisplacementMethod,
      cyclesMaterialUseMis,
      cyclesMaterialUseTransparentShadow,
      cyclesMaterialHeterogeneousVolume,
      cyclesMaterialVolumeSamplingMethod,
      cyclesMaterialVolumeInterpolationMethod,
      cyclesMaterialVolumeStepRate,
      cyclesBackgroundAoFactor,
      cyclesBackgroundAoDistance,
      cyclesBackgroundUseShader,
      cyclesBackgroundUseAo,
      cyclesBackgroundTransparent,
      cyclesBackgroundTransparentGlass,
      cyclesBackgroundTransparentRoughnessThreshold,
      cyclesBackgroundVolumeStepSize,
      cyclesBackgroundVisibilityCamera,
      cyclesBackgroundVisibilityDiffuse,
      cyclesBackgroundVisibilityGlossy,
      cyclesBackgroundVisibilityTransmission,
      cyclesBackgroundVisibilityScatter,
      cyclesObjectPointStyle,
      cyclesObjectPointResolution,
      cyclesBakeEnable,
      cyclesBakeObject,
      cyclesBakeBakeType,
      cyclesBakeDirect,
      cyclesBakeIndirect,
      cyclesBakeFilterColor,
      cyclesBakeFilterDiffuse,
      cyclesBakeFilterGlossy,
      cyclesBakeFilterTransmission,
      cyclesBakeFilterAmbientOcclusion,
      cyclesBakeFilterEmission,
      cyclesBakeMargin,
    })
{}

TfStaticData<HdCyclesTokensType> HdCyclesTokens;

WABI_NAMESPACE_END
