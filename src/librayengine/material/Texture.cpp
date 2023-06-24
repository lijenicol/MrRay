#include "ray/rt_stb_image.h"
#include "ray/geom/Hittable.h"
#include "ray/material/Texture.h"

RAY_NAMESPACE_OPEN_SCOPE

Colour NormalTexture::value(double u, double v, const hit_record& rec) const {
    return (rec.normal + Vec3(1, 1, 1)) / 2.0;
}

Colour CheckerTexture::value(double u, double v, const hit_record& rec) const {
    // Taking the sign of the sines gives us the checker texture
    double frequency = 8.0;
    double sines = sin(frequency*u*(2*pi)) * sin(frequency *v*(2*pi));
    if (sines < 0)
        return odd->value(u, v, rec);
    return even->value(u, v, rec);
}

ImageTexture::ImageTexture(const char* path) {
    int components_per_pixel = bytes_per_pixel;

    // Load image through stb_image 
    std::cerr << "Loading Image: " << path << "\n";
    data = stbi_load(
        path, &width, &height, &components_per_pixel, components_per_pixel);

    // Handle incorrect path
    if (!data) {
        std::cerr << "ERROR: Could not load path: " << path << ".\n";
        width = height = 0;
    }

    bytes_per_scanline = components_per_pixel * width;
}

Colour ImageTexture::value(double u, double v, const hit_record& rec) const {
    // Return solid cyan if no image
    if(data == nullptr)
        return Colour(0.5, 0, 0.5);
    
    // Get pixel coordinates
    u = clamp(u, 0, 1);
    v = 1.0 - clamp(v, 0, 1);

    int i = static_cast<int>(u * width);
    int j = static_cast<int>(v * height);

    // Keeps i & j below the width and height (due to 0 index)
    if (i >= width) i = width - 1;
    if (j >= height) j = height - 1;

    // Output pixel colour
    const double colour_scale = 1.0 / 255.0;
    unsigned char* pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;
    return Colour(colour_scale * pixel[0], colour_scale * pixel[1], colour_scale * pixel[2]);
}

RAY_NAMESPACE_CLOSE_SCOPE
