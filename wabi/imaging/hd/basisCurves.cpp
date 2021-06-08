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
#include "wabi/imaging/hd/basisCurves.h"
#include "wabi/base/tf/envSetting.h"
#include "wabi/imaging/hd/tokens.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

TF_DEFINE_PUBLIC_TOKENS(HdBasisCurvesReprDescTokens, HD_BASISCURVES_REPR_DESC_TOKENS);

TF_DEFINE_ENV_SETTING(HD_ENABLE_REFINED_CURVES, 0, "Force curves to always be refined.");

HdBasisCurves::HdBasisCurves(SdfPath const &id) : HdRprim(id)
{
  /*NOTHING*/
}

HdBasisCurves::~HdBasisCurves()
{
  /*NOTHING*/
}

/* virtual */
TfTokenVector const &HdBasisCurves::GetBuiltinPrimvarNames() const
{
  static const TfTokenVector primvarNames = {
      HdTokens->points, HdTokens->normals, HdTokens->widths};
  return primvarNames;
}

// static repr configuration
HdBasisCurves::_BasisCurvesReprConfig HdBasisCurves::_reprDescConfig;

/* static */
bool HdBasisCurves::IsEnabledForceRefinedCurves()
{
  return TfGetEnvSetting(HD_ENABLE_REFINED_CURVES) == 1;
}

/* static */
void HdBasisCurves::ConfigureRepr(TfToken const &reprName, HdBasisCurvesReprDesc desc)
{
  HD_TRACE_FUNCTION();

  if (IsEnabledForceRefinedCurves()) {
    desc.geomStyle = HdBasisCurvesGeomStylePatch;
  }

  _reprDescConfig.AddOrUpdate(reprName, _BasisCurvesReprConfig::DescArray{desc});
}

/* static */
HdBasisCurves::_BasisCurvesReprConfig::DescArray HdBasisCurves::_GetReprDesc(
    TfToken const &reprName)
{
  return _reprDescConfig.Find(reprName);
}

WABI_NAMESPACE_END
