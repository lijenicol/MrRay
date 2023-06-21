cmake_minimum_required(VERSION 3.23.0)
project(Ray)

add_library(rayengine SHARED)
add_executable(ray "")
target_link_libraries(ray PRIVATE rayengine)

# Setup public headers
file(GLOB_RECURSE RAY_PUBLIC_HEADER_FILES "src/*.h")
target_sources(rayengine PUBLIC FILE_SET HEADERS
    BASE_DIRS src
    FILES ${RAY_PUBLIC_HEADER_FILES})

install(TARGETS rayengine ray
        FILE_SET HEADERS)

add_subdirectory(src)
