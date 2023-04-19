cmake_minimum_required(VERSION 3.13.0)
project(Ray)

add_executable(ray "")
target_include_directories(ray PRIVATE ${ARGPARSE_INCLUDE_DIR})

add_subdirectory(src)
