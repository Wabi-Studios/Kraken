set(WABI_PREFIX wabi/usdImaging)
set(WABI_PACKAGE usdImagingGL)

if(NOT ${WITH_GPU_SUPPORT})
  message(STATUS "Skipping ${WABI_PACKAGE} because WITH_GPU_SUPPORT is OFF")
  return()
endif()

# Note: both python include directives are included for compatibility.
wabi_library(usdImagingGL
    LIBRARIES
        gf
        tf
        plug
        trace
        vt
        work
        hio
        garch
        glf
        hd
        hdx
        pxOsd
        sdf
        sdr
        usd
        usdGeom
        usdHydra
        usdShade
        usdImaging
        ar
        ${Boost_PYTHON_LIBRARY}
        ${PYTHON_LIBRARIES}
        ${TBB_LIBRARIES}

    INCLUDE_DIRS
        ${PYTHON_INCLUDE_PATH}
        ${PYTHON_INCLUDE_DIRS}
        ${TBB_INCLUDE_DIRS}

    PUBLIC_CLASSES
        drawModeAdapter
        engine
        package
        renderParams

    PUBLIC_HEADERS
        api.h
        rendererSettings.h
        version.h

    PRIVATE_CLASSES
        legacyEngine

    PRIVATE_HEADERS
        unitTestGLDrawing.h

    PYTHON_CPPFILES
        moduleDeps.cpp

    PYMODULE_CPPFILES
        module.cpp
        wrapEngine.cpp
        wrapRenderParams.cpp
        wrapRendererCommands.cpp
        wrapRendererSettings.cpp

    PYMODULE_FILES
        python/__init__.py

    RESOURCE_FILES
        resources/plugInfo.json
        resources/shaders/drawMode.glslfx
)
