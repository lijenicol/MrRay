add_library(mrRayEngine SHARED)

target_sources(mrRayEngine
    PRIVATE 
        AABB.cpp
        Colour.cpp
        film.cpp
        scene.cpp
        timer.cpp
        renderEngine.cpp
)

target_include_directories(mrRayEngine PUBLIC ../include)

file(GLOB_RECURSE MR_RAY_PUBLIC_HEADER_FILES "../include/*.h")
target_sources(mrRayEngine PUBLIC FILE_SET HEADERS
    BASE_DIRS ../include
    FILES ${MR_RAY_PUBLIC_HEADER_FILES})

find_package(OpenImageIO REQUIRED)
target_include_directories(mrRayEngine PRIVATE ${OpenImageIO_INCLUDE_DIR})
target_link_libraries(mrRayEngine PRIVATE OpenImageIO::OpenImageIO)

set_target_properties(mrRayEngine PROPERTIES OUTPUT_NAME "mrrayengine")
install(TARGETS mrRayEngine FILE_SET HEADERS)

add_subdirectory(geom)
add_subdirectory(material)
