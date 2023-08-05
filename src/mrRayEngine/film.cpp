#include "mrRay/film.h"

#include <memory>

#include <OpenImageIO/imageio.h>

#include "mrRay/Colour.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

void Film::writeTile(const Tile& tile)
{
    std::unique_lock<std::mutex> lock(filmMutex);
    for (size_t j = 0; j < tile.height; ++j)
    {
        for (size_t i = 0; i < tile.width; ++i)
        {
            size_t filmIndex = (j + tile.top) * this->width + i + tile.left;
            size_t tileIndex = j * tile.width + i;
            _colours[filmIndex] = tile.colours[tileIndex];
        }
    }
}

void Film::writeToFile(const std::string& path)
{
    std::unique_lock<std::mutex> lock(filmMutex);

    std::unique_ptr<OIIO::ImageOutput> out = OIIO::ImageOutput::create(path);
    std::cout << "Writing output to file: " << path << std::endl;
    if (!out)
    {
        std::cout << "Could not instantiate ImageOutput" << std::endl;
        return;
    }
    OIIO::ImageSpec spec(width, height, 3, OIIO::TypeDesc::FLOAT);
    out->open(path, spec);
    out->write_image(OIIO::TypeDesc::DOUBLE, 
                     _colours,
                     sizeof(Colour),
                     OIIO::AutoStride,
                     OIIO::AutoStride);
    out->close();
}

Colour*
Film::getData()
{
    return _colours;
}

MR_RAY_NAMESPACE_CLOSE_SCOPE
