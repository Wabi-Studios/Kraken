add_subdirectory(garch)
add_subdirectory(hf)
add_subdirectory(hio)
add_subdirectory(cameraUtil)
add_subdirectory(pxOsd)
add_subdirectory(glf)
add_subdirectory(hgi)
add_subdirectory(hgiGL)
add_subdirectory(hioOpenVDB)

if(WITH_METAL)
  add_subdirectory(hgiMetal)
endif()

if(WITH_DIRECTX)
  add_subdirectory(hgiDX3D)
endif()

if(WITH_VULKAN)
  add_subdirectory(hgiVulkan)
endif()
add_subdirectory(hgiInterop)
add_subdirectory(hd)

if(WITH_MATERIALX)
  add_subdirectory(hdMtlx)
endif()

add_subdirectory(hdSt)
add_subdirectory(hdx)

if(WITH_PRORENDER)
  add_subdirectory(rprUsd)
endif()

add_subdirectory(plugin)
