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

#include "wabi/base/tf/diagnosticBase.h"
#include "wabi/wabi.h"

#include "wabi/base/tf/diagnosticMgr.h"
#include "wabi/base/tf/registryManager.h"
#include "wabi/base/tf/stringUtils.h"

#include <string>

using std::string;

WABI_NAMESPACE_BEGIN

TfDiagnosticBase::TfDiagnosticBase(TfEnum code,
                                   const char *codeString,
                                   TfCallContext const &context,
                                   const string &commentary,
                                   TfDiagnosticInfo info,
                                   bool quiet)
    : _context(context)
{
  _commentary = commentary;
  _code       = code;
  _codeString = TfEnum::GetName(code);
  _info       = info;
  _quiet      = quiet;

  if (_codeString.empty())
    _codeString = TfSafeString(codeString);
}

string TfDiagnosticBase::GetPrettyPrintString() const
{
  return _commentary;
}

bool TfDiagnosticBase::IsFatal() const
{
  TfEnum code = _code;
  return (code == TF_DIAGNOSTIC_FATAL_CODING_ERROR_TYPE) ||
         (code == TF_DIAGNOSTIC_FATAL_ERROR_TYPE) || (code == TF_APPLICATION_EXIT_TYPE);
}

bool TfDiagnosticBase::IsCodingError() const
{
  TfEnum code = _code;
  return (code == TF_DIAGNOSTIC_CODING_ERROR_TYPE) ||
         (code == TF_DIAGNOSTIC_FATAL_CODING_ERROR_TYPE);
}

WABI_NAMESPACE_END
