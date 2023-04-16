target_sources(ray 
    PRIVATE 
        AABB.cpp
        AABB.h
        Camera.h
        Colour.cpp
        Colour.h
        Main.cpp
        ONB.h
        PDF.h
        Ray.h
        rt_stb_image.h
        rtutils.h
        Vec3.h
)

add_subdirectory(geom)
add_subdirectory(material)
