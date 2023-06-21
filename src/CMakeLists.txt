target_sources(rayengine
    PRIVATE 
        AABB.cpp
        Colour.cpp
        film.cpp
        scene.cpp
        timer.cpp
        renderEngine.cpp
)

target_include_directories(rayengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(OpenImageIO REQUIRED)
target_include_directories(rayengine PRIVATE ${OpenImageIO_INCLUDE_DIR})
target_link_libraries(rayengine PRIVATE OpenImageIO::OpenImageIO)

target_sources(ray PRIVATE Main.cpp)
target_include_directories(ray PRIVATE ${ARGPARSE_INCLUDE_DIR})

add_subdirectory(geom)
add_subdirectory(material)
