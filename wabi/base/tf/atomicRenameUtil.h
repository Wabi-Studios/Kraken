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
#include "wabi/wabi.h"

#include <string>

WABI_NAMESPACE_BEGIN

// Atomically rename \p srcFileName over \p dstFileName, assuming they are
// sibling files on the same filesystem.  Set \p error and return false in case
// of an error, otherwise return true.
bool Tf_AtomicRenameFileOver(std::string const &srcFileName,
                             std::string const &dstFileName,
                             std::string *error);

// Attempt to create a temporary sibling file of \p fileName.  If succesful
// return the realpath of \p fileName in \p realFileName, the created temporary
// file name in \p tempFileName, and its open file descriptor.  In case of an
// error, set \p error and return -1.
int Tf_CreateSiblingTempFile(std::string fileName,
                             std::string *realFileName,
                             std::string *tempFileName,
                             std::string *error);

WABI_NAMESPACE_END
