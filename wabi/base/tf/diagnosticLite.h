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
#ifndef WABI_BASE_TF_DIAGNOSTIC_LITE_H
#define WABI_BASE_TF_DIAGNOSTIC_LITE_H

/// \file tf/diagnosticLite.h
/// Stripped down version of \c diagnostic.h that doesn't define \c std::string.
///
/// This file provides the same functionality as \c diagnostic.h, except that
/// all strings must be passed as plain \c const \c char*, and not by
/// \c std::string, and the macro \c TF_FUNCTION_NAME() is only defined by
/// \c diagnostic.h
///
/// In particular, this header file does not include the C++ header file
/// \c < \c string \c >, making inclusion of this file a very light-weight
/// addition. Include this file, as opposed to pxr/base/tf/diagnostic.h in
/// header files that need to remain as light-weight as possible.
///
/// These macros are safe to use in multiple threads.

#include "wabi/wabi.h"
#include "wabi/base/arch/attributes.h"
#include "wabi/base/tf/api.h"
#include "wabi/base/arch/buildMode.h"
#include "wabi/base/arch/hints.h"
#include "wabi/base/tf/callContext.h"

#include <stddef.h>

WABI_NAMESPACE_BEGIN

/**
 * @enum TfDiagnosticType
 * Enum describing various
 * diagnostic conditions.
 *
 * These are ordered by enum
 * value of MSDOS's ability
 * to color console messages
 * w/ SetConsoleTextAttribute
 *
 * Those color values are as
 * follows:
 *
 * 1: Blue
 * 2: Green
 * 3: Cyan
 * 4: Red
 * 5: Pink
 * 6: Yellow
 * 7: White
 * 8: Gray
 *
 * Then, these color values repeat
 * with higher vibrancies, ex:
 *
 * 9: Light Blue
 * 10: Light Green
 * 11: Light Cyan
 * etc... */
enum TfDiagnosticType : int
{
  /** Color Invalid - Black */
  TF_DIAGNOSTIC_INVALID_TYPE = 0,
  /** Color Blue */
  TF_DIAGNOSTIC_CODING_ERROR_TYPE,
  /** Color Green (User Friendly) */
  TF_DIAGNOSTIC_MSG_SUCCESS_TYPE,
  /** Color Cyan */
  TF_DIAGNOSTIC_FATAL_CODING_ERROR_TYPE,
  /** Color Red (User Friendly) */
  TF_DIAGNOSTIC_MSG_ERROR_TYPE,
  /** Color Pink */
  TF_DIAGNOSTIC_RUNTIME_ERROR_TYPE,
  /** Color Yellow (User Friendly) */
  TF_DIAGNOSTIC_MSG_WARNING_TYPE,
  /** Color White (User Friendly) */
  TF_DIAGNOSTIC_MSG_TYPE,
  /** Color Gray */
  TF_DIAGNOSTIC_STATUS_TYPE,
  /** Color Light Blue */
  TF_DIAGNOSTIC_NONFATAL_ERROR_TYPE,
  /** Color Light Green */
  TF_DIAGNOSTIC_FATAL_ERROR_TYPE,
  /** Color Light Cyan */
  TF_DIAGNOSTIC_WARNING_TYPE,
  /** Color Light Red */
  TF_APPLICATION_EXIT_TYPE,
};


#if !defined(doxygen)

struct Tf_DiagnosticLiteHelper
{
  constexpr Tf_DiagnosticLiteHelper(TfCallContext const &context, TfDiagnosticType type)
    : _context(context),
      _type(type)
  {}

  TF_API void IssueError(char const *fmt, ...) const ARCH_PRINTF_FUNCTION(2, 3);
  TF_API void IssueFatalError(char const *fmt, ...) const ARCH_PRINTF_FUNCTION(2, 3);
  TF_API void IssueWarning(char const *fmt, ...) const ARCH_PRINTF_FUNCTION(2, 3);
  TF_API void IssueStatus(char const *fmt, ...) const ARCH_PRINTF_FUNCTION(2, 3);

 private:

  TfCallContext _context;
  TfDiagnosticType _type;
};

#  define TF_CODING_ERROR \
    Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_CODING_ERROR_TYPE).IssueError

#  define TF_CODING_WARNING \
    Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_CODING_ERROR_TYPE).IssueWarning

#  define TF_FATAL_CODING_ERROR \
    Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_CODING_ERROR_TYPE).IssueFatalError

#  define TF_RUNTIME_MSG \
    Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_MSG_TYPE).IssueStatus

#  define TF_RUNTIME_MSG_SUCCESS \
    Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_MSG_SUCCESS_TYPE).IssueStatus

#  define TF_RUNTIME_MSG_ERROR \
    Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_MSG_ERROR_TYPE).IssueError

#  define TF_RUNTIME_MSG_WARNING \
    Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_MSG_WARNING_TYPE).IssueWarning

#  define TF_RUNTIME_ERROR \
    Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_RUNTIME_ERROR_TYPE).IssueError

#  define TF_FATAL_ERROR \
    Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_FATAL_ERROR_TYPE).IssueFatalError

#  define TF_DIAGNOSTIC_FATAL_ERROR \
    Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_RUNTIME_ERROR_TYPE).IssueFatalError

#  define TF_DIAGNOSTIC_NONFATAL_ERROR \
    Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_WARNING_TYPE).IssueWarning

#  define TF_DIAGNOSTIC_WARNING \
    Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT.Hide(), TF_DIAGNOSTIC_WARNING_TYPE).IssueWarning

#  define TF_WARN Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_WARNING_TYPE).IssueWarning

#  define TF_STATUS Tf_DiagnosticLiteHelper(TF_CALL_CONTEXT, TF_DIAGNOSTIC_STATUS_TYPE).IssueStatus

constexpr bool Tf_AxiomHelper(bool val, TfCallContext const &ctx, char const *txt)
{
  return (ARCH_LIKELY(val)) ? true :
                              (Tf_DiagnosticLiteHelper(ctx, TF_DIAGNOSTIC_FATAL_ERROR_TYPE)
                                 .IssueFatalError("Failed axiom: ' %s '", txt),
                               false);
}

#  define TF_AXIOM(cond) Tf_AxiomHelper(static_cast<bool>((cond)), TF_CALL_CONTEXT, #  cond)

#  define TF_DEV_AXIOM(cond) \
    Tf_AxiomHelper(!ARCH_DEV_BUILD || static_cast<bool>((cond)), TF_CALL_CONTEXT, #cond)

#endif  // !defined(doxygen)

WABI_NAMESPACE_END

#endif  // WABI_BASE_TF_DIAGNOSTIC_LITE_H
