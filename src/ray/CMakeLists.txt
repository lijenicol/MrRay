if(${RAY_BUILD_DEMO})

add_executable(ray "")
target_sources(ray PRIVATE ray.cpp)
target_include_directories(ray PRIVATE ${ARGPARSE_INCLUDE_DIR})
target_link_libraries(ray PRIVATE rayengine)

install(TARGETS ray)

endif()