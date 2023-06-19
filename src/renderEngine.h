//
// Created by lijenicol on 19/06/23.
//

#ifndef DEV_RENDERENGINE_H
#define DEV_RENDERENGINE_H

#include <string>

#include "film.h"
#include "memory.h"
#include "sampler.h"
#include "scene.h"

struct RenderSettings
{
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int samplesPerPixel;
    unsigned int threads;
    unsigned int tileSize;
    std::string outputPath;

    RenderSettings(unsigned int w, unsigned int h, unsigned int spp,
                   unsigned int threads, unsigned int tileSize,
                   const std::string& outputPath)
        : imageWidth(w), imageHeight(h), samplesPerPixel(spp),
          threads(threads), tileSize(tileSize), outputPath(outputPath)
    {}

    RenderSettings(const RenderSettings& other)
            : imageWidth(other.imageWidth), imageHeight(other.imageHeight),
              samplesPerPixel(other.samplesPerPixel), threads(other.threads),
              tileSize(other.tileSize)
    {}

    double aspectRatio() const { return imageWidth / (double)imageHeight; }
};

struct ExecutionBlock
{
    const unsigned int blockID;
    const RenderSettings renderSettings;
    Sampler sampler;
    MemoryArena arena;

    ExecutionBlock(unsigned int blockID, const RenderSettings& renderSettings)
        : blockID(blockID), renderSettings(renderSettings), arena(),
          sampler(blockID)
    {};

    void execute(const Scene& scene, const Tile& tile);
};

class RenderEngine
{
public:
    RenderEngine();
    ~RenderEngine();

    // Sets up film/tile buffers. Required before running execute()
    void init(const RenderSettings& renderSettings);
    // Runs the execution
    void execute(const RenderSettings& renderSettings, const Scene& scene);
// TODO:
//    void registerRenderFinishedCallback();
//    void registerTileFinishedCallback();

    std::shared_ptr<Film> getFilm() { return _film; }

private:
    std::shared_ptr<Film> _film;
    std::shared_ptr<TilesQueue> _tilesQueue;
    bool _hasInitialized;

    RenderEngine(const RenderEngine&) = delete;
    RenderEngine& operator=(const RenderEngine&) = delete;
};

#endif //DEV_RENDERENGINE_H
