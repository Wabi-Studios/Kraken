set(WABI_PREFIX wabi/imaging)
set(WABI_PACKAGE hgiInterop)

if(NOT ${WITH_GPU_SUPPORT})
  message(STATUS "Skipping ${WABI_PACKAGE} because WITH_GPU_SUPPORT is OFF")
  return()
endif()

set(optionalLibraries "")
set(optionalCppFiles "")
set(optionalPrivateHeaders "")

if(WITH_VULKAN OR WITH_OPENGL OR WITH_METAL OR WITH_DIRECTX)
  list(APPEND optionalLibraries garch)
endif()

if(WITH_METAL)
  add_compile_options(-x objective-c++)
  FIND_LIBRARY(COREVIDEO_LIBRARY CoreVideo)
  list(APPEND optionalLibraries ${COREVIDEO_LIBRARY} hgiMetal)
  list(APPEND optionalCppFiles metal.mm)
  list(APPEND optionalPrivateHeaders metal.h)
else()
  # No OpenGL-to-OpenGL interop when using Metal.
  list(APPEND optionalCppFiles opengl.cpp)
  list(APPEND optionalPrivateHeaders opengl.h)
endif()

if(WITH_OPENGL)
  list(APPEND optionalLibraries hgiGL)
  list(APPEND optionalCppFiles opengl.cpp)
  list(APPEND optionalPrivateHeaders opengl.h)
endif()

if(WITH_VULKAN)
  list(APPEND optionalLibraries hgiVulkan)
  list(APPEND optionalCppFiles vulkan.cpp)
  list(APPEND optionalPrivateHeaders vulkan.h)
endif()

if(WITH_DIRECTX)
  list(APPEND optionalLibraries hgiDX3D)
  list(APPEND optionalCppFiles dx3d.cpp)
  list(APPEND optionalPrivateHeaders dx3d.h)
endif()
wabi_library(hgiInterop
    LIBRARIES
        gf
        tf
        hgi
        vt
        ${optionalLibraries}

    PUBLIC_HEADERS
        api.h
        hgiInterop.h

    PRIVATE_HEADERS
        ${optionalPrivateHeaders}

    CPPFILES
        hgiInterop.cpp
        ${optionalCppFiles}
)
