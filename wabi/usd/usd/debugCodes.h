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
#ifndef WABI_USD_USD_DEBUG_CODES_H
#define WABI_USD_USD_DEBUG_CODES_H

#include "wabi/base/tf/debug.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

TF_DEBUG_CODES(USD_AUTO_APPLY_API_SCHEMAS,
               USD_CHANGES,
               USD_CLIPS,
               USD_COMPOSITION,
               USD_DATA_BD,
               USD_DATA_BD_TRY,
               USD_INSTANCING,
               USD_PATH_RESOLUTION,
               USD_PAYLOADS,
               USD_PRIM_LIFETIMES,
               USD_SCHEMA_REGISTRATION,
               USD_STAGE_CACHE,
               USD_STAGE_LIFETIMES,
               USD_STAGE_OPEN,
               USD_STAGE_INSTANTIATION_TIME,
               USD_VALUE_RESOLUTION,
               USD_VALIDATE_VARIABILITY

);

WABI_NAMESPACE_END

#endif  // WABI_USD_USD_DEBUG_CODES_H
