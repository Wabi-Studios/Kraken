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
#ifndef WABI_BASE_TF_ERROR_TRANSPORT_H
#define WABI_BASE_TF_ERROR_TRANSPORT_H

/// \file tf/errorTransport.h

#include "wabi/base/arch/hints.h"
#include "wabi/base/tf/api.h"
#include "wabi/base/tf/diagnosticMgr.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

/// \class TfErrorTransport
///
/// A facility for transporting errors from thread to thread.
///
/// Typical use is to create a TfErrorMark in the thread that is the error
/// source (e.g. the child thread), then call TfErrorMark::Transport() or
/// TfErrorMark::TransportTo() to lift generated errors out into a
/// TfErrorTransport object.  Later the thread that wants to sink those errors
/// (e.g. the parent thread) invokes TfErrorTransport::Post() to post all
/// contained errors to its own thread's error list.
class TfErrorTransport {
 public:
  typedef TfDiagnosticMgr::ErrorList ErrorList;

  /// Construct an empty TfErrorTransport.
  TfErrorTransport()
  {}

  /// Post all contained errors to the current thread's error list, leaving
  /// this TfErrorTransport empty.
  void Post()
  {
    if (ARCH_UNLIKELY(!IsEmpty()))
      _PostImpl();
  }

  /// Return true if this TfErrorTransport contains no errors, false
  /// otherwise.
  bool IsEmpty() const
  {
    return _errorList.empty();
  }

  /// Swap this TfErrorTransport's content with \p other.  This provides a
  /// lightweight way to move the contents of one TfErrorTransport to
  /// another.
  void swap(TfErrorTransport &other)
  {
    _errorList.swap(other._errorList);
  }

 private:
  friend class TfErrorMark;

  TfErrorTransport(ErrorList &src, ErrorList::iterator first, ErrorList::iterator last)
  {
    _errorList.splice(_errorList.begin(), src, first, last);
  }

  TF_API
  void _PostImpl();

  ErrorList _errorList;
};

inline void swap(TfErrorTransport &l, TfErrorTransport &r)
{
  l.swap(r);
}

WABI_NAMESPACE_END

#endif  // WABI_BASE_TF_ERROR_TRANSPORT_H
