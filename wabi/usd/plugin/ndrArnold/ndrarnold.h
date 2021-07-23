// Copyright 2019 Autodesk, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#define ARNOLD_XSTR(s) ARNOLD_STR(s)
#define ARNOLD_STR(s) #s

#define AI_VERSION_ARCH_NUM 6
#define AI_VERSION_MAJOR_NUM 2
#define AI_VERSION_MINOR_NUM 1

#define AI_VERSION_ARCH ARNOLD_XSTR(AI_VERSION_ARCH_NUM)
#define AI_VERSION_MAJOR ARNOLD_XSTR(AI_VERSION_MAJOR_NUM)
#define AI_VERSION_MINOR ARNOLD_XSTR(AI_VERSION_MINOR_NUM)

#define AI_VERSION_NUMBER (AI_VERSION_ARCH_NUM * 10000 + AI_VERSION_MAJOR_NUM * 100 + AI_VERSION_MINOR_NUM)

#define USED_USD_MAJOR_VERSION 0
#define USED_USD_MINOR_VERSION 20
#define USED_USD_PATCH_VERSION 11

#define USED_USD_VERSION_GREATER_EQ(x, y)                                                       \
  (USED_USD_MINOR_VERSION >= x && USED_USD_PATCH_VERSION >= y) || USED_USD_MINOR_VERSION > x || \
    USED_USD_MAJOR_VERSION > 0

#define USD_HAS_NEW_SDR_NODE_CONSTRUCTOR
