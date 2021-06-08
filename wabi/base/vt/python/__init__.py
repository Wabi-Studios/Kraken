# 
#  Copyright 2021 Pixar. All Rights Reserved.
# 
#  Portions of this file are derived from original work by Pixar
#  distributed with Universal Scene Description, a project of the
#  Academy Software Foundation (ASWF). https://www.aswf.io/
# 
#  Licensed under the Apache License, Version 2.0 (the "Apache License")
#  with the following modification; you may not use this file except in
#  compliance with the Apache License and the following modification:
#  Section 6. Trademarks. is deleted and replaced with:
# 
#  6. Trademarks. This License does not grant permission to use the trade
#     names, trademarks, service marks, or product names of the Licensor
#     and its affiliates, except as required to comply with Section 4(c)
#     of the License and to reproduce the content of the NOTICE file.
#
#  You may obtain a copy of the Apache License at:
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the Apache License with the above modification is
#  distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
#  ANY KIND, either express or implied. See the Apache License for the
#  specific language governing permissions and limitations under the
#  Apache License.
#
#  Modifications copyright (C) 2020-2021 Wabi.
#
"""Vt Value Types library

This package defines classes for creating objects that behave like value
types.  It contains facilities for creating simple copy-on-write
implicitly shared types.
"""

from . import _vt
from wabi import Tf
Tf.PrepareModule(_vt, locals())
del _vt, Tf

try:
    from . import __DOC
    __DOC.Execute(locals())
    del __DOC
except Exception:
    pass

# For ease-of-use, put each XXXArrayFromBuffer method on its corresponding array
# wrapper class, also alias it as 'FromNumpy' for compatibility.
def _CopyArrayFromBufferFuncs(moduleContents):
    funcs = dict([(key, val) for (key, val) in moduleContents.items()
                  if key.endswith('FromBuffer')])
    classes = dict([(key, val) for (key, val) in moduleContents.items()
                    if key.endswith('Array') and isinstance(val, type)])

    for funcName, func in funcs.items():
        className = funcName[:-len('FromBuffer')]
        cls = classes.get(className)
        if cls:
            setattr(cls, 'FromBuffer', staticmethod(func))
            setattr(cls, 'FromNumpy', staticmethod(func))

_CopyArrayFromBufferFuncs(dict(vars()))
del _CopyArrayFromBufferFuncs
