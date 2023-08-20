if(${MR_RAY_BUILD_HYDRA})

add_library(hdMrRay SHARED)

find_package(pxr REQUIRED)

target_include_directories(
    hdMrRay
    PRIVATE
    ${PXR_INCLUDE_DIRS}
)
target_link_libraries(hdMrRay PRIVATE hd mrRayEngine)

target_sources(hdMrRay
    PRIVATE
        config.cpp
        mesh.cpp
        renderBuffer.cpp
        renderDelegate.cpp
        renderPass.cpp
        rendererPlugin.cpp
        renderer.cpp
)

set_target_properties(hdMrRay PROPERTIES PREFIX "")

install(TARGETS hdMrRay DESTINATION plugin)
install(FILES plugInfo.json DESTINATION plugin)

endif()