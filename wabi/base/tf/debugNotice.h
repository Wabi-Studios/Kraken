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
#ifndef WABI_BASE_TF_DEBUG_NOTICE_H
#define WABI_BASE_TF_DEBUG_NOTICE_H

/// \file tf/debugNotice.h

#include "wabi/wabi.h"
#include "wabi/base/tf/notice.h"

WABI_NAMESPACE_BEGIN

/// \class TfDebugSymbolsChangedNotice
///
/// Sent when the list of available debug symbol names has changed.
class TfDebugSymbolsChangedNotice : public TfNotice
{
 public:

  TfDebugSymbolsChangedNotice() {}
  virtual ~TfDebugSymbolsChangedNotice();
};

/// \class TfDebugSymbolEnableChangedNotice
///
/// Sent when a debug symbol has been enabled or disabled.
class TfDebugSymbolEnableChangedNotice : public TfNotice
{
 public:

  TfDebugSymbolEnableChangedNotice() {}
  virtual ~TfDebugSymbolEnableChangedNotice();
};

WABI_NAMESPACE_END

#endif
