#include "mrRay/renderEngine.h"

#include <thread>
#include <vector>

#include "mrRay/geom/sphere.h"
#include "mrRay/material/material.h"
#include "mrRay/pdf.h"
#include "mrRay/timer.h"

#define MAX_DEPTH 12

MR_RAY_NAMESPACE_OPEN_SCOPE

Colour
rayColourHelper(
    const Ray &r, const Scene &scene, MemoryArena &arena, Sampler &sampler,
    int depth)
{
    // Rays that bounce between white objects may never terminate
    // from the russian roulette process, this is a fail-safe
    if (depth > MAX_DEPTH) {
        return Colour(0, 0, 0);
    }

    // If the ray hits nothing, return the skybox colour
    hit_record rec;
    if (!scene.getWorld()->hit(r, 0.001, infinity, rec)) {
        // Compute u,v of hit
        double u, v;
        Sphere::get_sphere_uv(unit_vector(r.direction()), u, v);
        return scene.getSkyboxTexture()->value(u, v, rec);
    }

    Colour emitted = rec.mat->emitted(0, 0, Vec3(0, 0, 0));
    scatter_record srec;
    if (!rec.mat->scatter(r, rec, srec, arena)) return emitted;

    // If the generated ray is specular, then we do not need to sample directions
    // (this is because the specular only has one possible scattering ray)
    if (srec.isSpecular) {
        return srec.attenuation
             * rayColourHelper(srec.specularRay, scene, arena, sampler, depth)
             * dot(rec.normal, srec.specularRay.direction());
    }

    // Determine russian roulette termination
    float pContinue = 1 - (srec.attenuation.length() / sqrt(3));
    pContinue = pContinue < 0 ? 0 : pContinue;
    float invpContinue = 1 / (1 - pContinue);
    float randomRussian = sampler.getDouble();
    if (randomRussian < pContinue) {
        return Colour(0, 0, 0);
    }

    // Generate sample direction
    double pdf = 0;
    Ray scattered;
    while (pdf == 0) {
        scattered = Ray(rec.p, srec.PDF_ptr->generate(sampler));
        pdf = srec.PDF_ptr->value(scattered.direction());
    }

    // Recursively scatter rays
    return emitted
         + srec.attenuation * rec.mat->bsdf(r, rec, scattered)
               * dot(rec.normal, scattered.direction())
               * rayColourHelper(scattered, scene, arena, sampler, depth + 1)
               * invpContinue / pdf;
}

Colour
ray_colour(const Ray &r, const Scene &scene, MemoryArena &arena, Sampler &sampler)
{
    return rayColourHelper(r, scene, arena, sampler, 0);
}

void
ExecutionBlock::execute(Scene *scene, const Tile &tile)
{
    Camera *mainCam = scene->getMainCam();
    for (unsigned int j = tile.top; j < tile.top + tile.height; j++) {
        for (unsigned int i = tile.left; i < tile.left + tile.width; i++) {
            Colour pixel_colour(0, 0, 0);
            for (unsigned int s = 0; s < renderSettings.samplesPerPixel; s++) {
                double u = (i + sampler.getDouble())
                         / (renderSettings.imageWidth - 1.0);
                double v = (j + sampler.getDouble())
                         / (renderSettings.imageHeight - 1.0);
                Ray r = mainCam->getRay(u, v, sampler);
                pixel_colour += ray_colour(r, *scene, arena, sampler);
            }
            unsigned int tileIndex = (j - tile.top) * tile.width + i - tile.left;
            tile.colours[tileIndex]
                = pixel_colour / renderSettings.samplesPerPixel;
        }
    }
}

void
executeBlock(
    std::shared_ptr<ExecutionBlock> block, Scene *scene,
    std::shared_ptr<Film> film, std::shared_ptr<TilesQueue> tilesQueue)
{
    Tile *tile = tilesQueue->getTile();
    while (tile != nullptr) {
        block->execute(scene, *tile);
        film->writeTile(*tile);
        tile = tilesQueue->getTile();
        block->arena.Reset();
    }
}

RenderEngine::RenderEngine()
    : _hasInitialized(false)
{
}

RenderEngine::~RenderEngine()
{
}

void
RenderEngine::init(const RenderSettings &renderSettings)
{
    _film = std::make_shared<Film>(
        renderSettings.imageWidth, renderSettings.imageHeight);

    unsigned int tileSize = renderSettings.tileSize;
    _tilesQueue = std::make_shared<TilesQueue>();

    unsigned int remainingHeight = _film->height;
    while (remainingHeight > 0) {
        unsigned int height
            = remainingHeight > tileSize ? tileSize : remainingHeight;
        unsigned int remainingWidth = _film->width;
        while (remainingWidth > 0) {
            unsigned int width
                = remainingWidth > tileSize ? tileSize : remainingWidth;
            _tilesQueue->addTile(std::make_shared<Tile>(
                _film->height - remainingHeight,
                _film->width - remainingWidth,
                width,
                height));
            remainingWidth -= width;
        }
        remainingHeight -= height;
    }

    _hasInitialized = true;
}

void
RenderEngine::execute(const RenderSettings &renderSettings, Scene *scene)
{
    if (!_hasInitialized) {
        std::cerr << "RenderEngine execution called before init" << std::endl;
        return;
    }

    if (!scene->getMainCam()) {
        std::cerr << "No camera to render from!" << std::endl;
        return;
    }

    {
        Timer timer("scene init");
        scene->init();
    }

    std::vector<std::thread> threads(renderSettings.threads);
    for (size_t i = 0; i < renderSettings.threads; ++i) {
        std::shared_ptr<ExecutionBlock> block
            = std::make_shared<ExecutionBlock>(i, renderSettings);
        threads[i] = std::thread(executeBlock, block, scene, _film, _tilesQueue);
    }

    for (std::thread &thread: threads) {
        thread.join();
    }
}

MR_RAY_NAMESPACE_CLOSE_SCOPE
