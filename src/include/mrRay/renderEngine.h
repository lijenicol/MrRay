//
// Created by lijenicol on 19/06/23.
//

#ifndef DEV_RENDERENGINE_H
#define DEV_RENDERENGINE_H

#include <string>

#include "mrRay/film.h"
#include "mrRay/memory.h"
#include "mrRay/namespace.h"
#include "mrRay/sampler.h"
#include "mrRay/scene.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

struct RenderSettings {
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int samplesPerPixel;
    unsigned int threads;
    unsigned int tileSize;

    RenderSettings(
        unsigned int w, unsigned int h, unsigned int spp, unsigned int threads,
        unsigned int tileSize)
        : imageWidth(w)
        , imageHeight(h)
        , samplesPerPixel(spp)
        , threads(threads)
        , tileSize(tileSize)
    {
    }

    RenderSettings(const RenderSettings &other)
        : imageWidth(other.imageWidth)
        , imageHeight(other.imageHeight)
        , samplesPerPixel(other.samplesPerPixel)
        , threads(other.threads)
        , tileSize(other.tileSize)
    {
    }

    double aspectRatio() const { return imageWidth / (double)imageHeight; }
};

struct ExecutionBlock {
    const unsigned int blockID;
    const RenderSettings renderSettings;
    Sampler sampler;
    MemoryArena arena;

    ExecutionBlock(unsigned int blockID, const RenderSettings &renderSettings)
        : blockID(blockID)
        , renderSettings(renderSettings)
        , arena()
        , sampler(blockID) {};

    void execute(Scene *scene, const Tile &tile);
};

class RenderEngine
{
public:
    RenderEngine();
    ~RenderEngine();

    // Sets up film/tile buffers. Required before running execute()
    void init(const RenderSettings &renderSettings);
    // Runs the execution
    void execute(const RenderSettings &renderSettings, Scene *scene);
    // TODO:
    //    void registerRenderFinishedCallback();
    //    void registerTileFinishedCallback();

    std::shared_ptr<Film> getFilm() { return _film; }

private:
    std::shared_ptr<Film> _film;
    std::shared_ptr<TilesQueue> _tilesQueue;
    bool _hasInitialized;

    RenderEngine(const RenderEngine &) = delete;
    RenderEngine &operator=(const RenderEngine &) = delete;
};

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif // DEV_RENDERENGINE_H
