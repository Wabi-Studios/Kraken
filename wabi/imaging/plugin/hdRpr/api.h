/************************************************************************
Copyright 2020 Advanced Micro Devices, Inc
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
************************************************************************/

#ifndef HDRPR_API_H
#define HDRPR_API_H

#include "wabi/base/arch/export.h"

#if defined(WABI_STATIC)
#  define HDRPR_API
#  define HDRPR_API_TEMPLATE_CLASS(...)
#  define HDRPR_API_TEMPLATE_STRUCT(...)
#  define HDRPR_LOCAL
#else
#  if defined(HDRPR_EXPORTS)
#    define HDRPR_API ARCH_EXPORT
#    define HDRPR_API_TEMPLATE_CLASS(...) ARCH_EXPORT_TEMPLATE(class, __VA_ARGS__)
#    define HDRPR_API_TEMPLATE_STRUCT(...) ARCH_EXPORT_TEMPLATE(struct, __VA_ARGS__)
#  else
#    define HDRPR_API ARCH_IMPORT
#    define HDRPR_API_TEMPLATE_CLASS(...) ARCH_IMPORT_TEMPLATE(class, __VA_ARGS__)
#    define HDRPR_API_TEMPLATE_STRUCT(...) ARCH_IMPORT_TEMPLATE(struct, __VA_ARGS__)
#  endif
#  define HDRPR_LOCAL ARCH_HIDDEN
#endif

#endif  // HDRPR_API_H
