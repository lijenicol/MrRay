add_library(rayengine SHARED)

target_sources(rayengine
    PRIVATE 
        AABB.cpp
        Colour.cpp
        film.cpp
        scene.cpp
        timer.cpp
        renderEngine.cpp
)

target_include_directories(rayengine PUBLIC ../include)

file(GLOB_RECURSE RAY_PUBLIC_HEADER_FILES "../include/*.h")
target_sources(rayengine PUBLIC FILE_SET HEADERS
    BASE_DIRS ../include
    FILES ${RAY_PUBLIC_HEADER_FILES})

find_package(OpenImageIO REQUIRED)
target_include_directories(rayengine PRIVATE ${OpenImageIO_INCLUDE_DIR})
target_link_libraries(rayengine PRIVATE OpenImageIO::OpenImageIO)

install(TARGETS rayengine
        FILE_SET HEADERS)

add_subdirectory(geom)
add_subdirectory(material)
