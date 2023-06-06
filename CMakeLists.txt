cmake_minimum_required(VERSION 3.13.0)
project(Ray)

add_executable(ray "")
find_package(OpenImageIO REQUIRED)
target_include_directories(
    ray 
        PRIVATE 
        ${ARGPARSE_INCLUDE_DIR} 
        ${OpenImageIO_INCLUDE_DIR}
)
target_link_libraries(ray PRIVATE OpenImageIO::OpenImageIO)

add_subdirectory(src)
