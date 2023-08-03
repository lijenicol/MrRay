if(${RAY_BUILD_HYDRA})

add_library(hdRay SHARED)

find_package(pxr REQUIRED)

target_include_directories(
    hdRay
    PRIVATE
    ${PXR_INCLUDE_DIRS}
)
target_link_libraries(hdRay PRIVATE hd rayengine)

target_sources(hdRay
    PRIVATE
        mesh.cpp
        renderBuffer.cpp
        renderDelegate.cpp
        renderPass.cpp
        rendererPlugin.cpp
        renderer.cpp
)

set_target_properties(hdRay PROPERTIES PREFIX "")

install(TARGETS hdRay DESTINATION plugin)
install(FILES plugInfo.json DESTINATION plugin)

endif()