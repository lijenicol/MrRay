if(${MR_RAY_BUILD_DEMO})

add_executable(mrRayDemo "")
target_sources(mrRayDemo PRIVATE demo.cpp)
target_include_directories(mrRayDemo PRIVATE ${ARGPARSE_INCLUDE_DIR})
target_link_libraries(mrRayDemo PRIVATE mrRayEngine)

install(TARGETS mrRayDemo)

endif()