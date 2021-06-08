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
#ifndef WABI_USD_PCP_PATH_TRANSLATION_H
#define WABI_USD_PCP_PATH_TRANSLATION_H

/// \file pcp/pathTranslation.h
/// Path translation.

#include "wabi/usd/pcp/api.h"
#include "wabi/wabi.h"

WABI_NAMESPACE_BEGIN

class PcpMapFunction;
class PcpNodeRef;
class SdfPath;

/// Translates \p pathInNodeNamespace from the namespace of the prim index node
/// \p sourceNode to the namespace of the prim index's root node. This applies
/// all necessary namespace translations.
///
/// If the path is successfully translated and \p pathWasTranslated is supplied,
/// it will be set to \c true. In some cases, paths may fail to translate
/// because they fall outside the set of paths that are allowed by nodes in the
/// prim index. For instance, for a referenced model, paths referring to
/// locations outside that model will not be translated. In these cases, this
/// function will return an empty SdfPath and \p pathWasTranslated will be set
/// to \c false.
///
/// In Sd/Csd terminology, this is forward path translation from the
/// namespace of the prim spec represented by \p sourceNode to the composed
/// scene namespace.
PCP_API
SdfPath PcpTranslatePathFromNodeToRoot(const PcpNodeRef &sourceNode,
                                       const SdfPath &pathInNodeNamespace,
                                       bool *pathWasTranslated = 0);

/// Translates \p pathInRootNamespace from the namespace of the root of the
/// prim index that \p destNode belongs to to the namespace of \p destNode
/// itself. This applies all necessary namespace translations.
///
/// If the path is successfully translated and \p pathWasTranslated is supplied,
/// it will be set to \c true. In some cases, paths may fail to translate
/// because they fall outside the set of paths that are allowed by nodes in the
/// prim index. For instance, for a referenced model, paths referring to
/// locations outside that model will not be translated. In these cases, this
/// function will return an empty SdfPath and \p pathWasTranslated will be set
/// to \c false.
///
/// In Sd/Csd terminology, this is reverse path translation from the
/// namespace of the composed scene to the namespace of the prim spec
/// represented by \p destNode.
PCP_API
SdfPath PcpTranslatePathFromRootToNode(const PcpNodeRef &destNode,
                                       const SdfPath &pathInRootNamespace,
                                       bool *pathWasTranslated = 0);

/// Same as \a PcpTranslatePathFromRootToNode, but explicitly for use when
/// translating paths intended for use as attribute connections or relationship
/// targets. The primary difference is that variant selections will never
/// be included in the translated path. This is functionally equivalent to
/// calling StripAllVariantSelections() on the result of the referenced
/// function, but is more efficient.
PCP_API
SdfPath PcpTranslateTargetPathFromRootToNode(const PcpNodeRef &destNode,
                                             const SdfPath &pathInRootNamespace,
                                             bool *pathWasTranslated = 0);

/// Convenience function like \a PcpTranslatePathFromRootToNode, but
/// takes a function rather than a node.
PCP_API
SdfPath PcpTranslatePathFromRootToNodeUsingFunction(const PcpMapFunction &mapToRoot,
                                                    const SdfPath &pathInRootNamespace,
                                                    bool *pathWasTranslated = 0);

/// Convenience function like \a PcpTranslatePathFromNodeToRoot, but
/// takes a function rather than a node.
PCP_API
SdfPath PcpTranslatePathFromNodeToRootUsingFunction(const PcpMapFunction &mapToRoot,
                                                    const SdfPath &pathInNodeNamespace,
                                                    bool *pathWasTranslated = 0);

WABI_NAMESPACE_END

#endif  // WABI_USD_PCP_PATH_TRANSLATION_H
