#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>

#include "argparse/argparse.hpp"

#include "rtutils.h"

#include "Camera.h"
#include "PDF.h"
#include "memory.h"
#include "sampler.h"
#include "scene.h"
#include "timer.h"
#include "film.h"

#include "geom/Sphere.h"

#include "material/Material.h"
#include "material/Texture.h"

// Recursive function for calculating intersections and colour
Colour ray_colour(const Ray& r, const Scene& scene, MemoryArena& arena, 
			      Sampler& sampler) {	
	// If the ray hits nothing, return the skybox colour
	hit_record rec;
	if (!scene.world->hit(r, 0.001, infinity, rec)) {
		// Compute u,v of hit
		double u, v;
		Sphere::get_sphere_uv(unit_vector(r.direction()), u, v);
		return scene.skyboxTexture->value(u,v,rec);
	}

	Colour emitted = rec.mat->emitted(0,0,Vec3(0,0,0));	// needs to change when u,v coordinates is implemented
	scatter_record srec;
	if (!rec.mat->scatter(r, rec, srec, arena))
		return emitted;

	// If the generated ray is specular, then we do not need to sample directions
	// (this is because the specular only has one possible scattering ray)
	if (srec.isSpecular) {
		return srec.attenuation 
			* ray_colour(srec.specularRay, scene, arena, sampler)
			* dot(rec.normal, srec.specularRay.direction());
	}

	// Determine russian roulette termination
	//float maxChannel = fmax(fmax(srec.attenuation[0], srec.attenuation[1]), srec.attenuation[2]);
	//float pContinue = 1 - maxChannel;
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
		// FIXME: Fix this now that lights aren't shared_ptr
		// std::shared_ptr<HittablePDF> lightPDF = std::make_shared<HittablePDF>(lights, rec.p);
		// MixturePDF mixPDF(srec.PDF_ptr, lightPDF);
		// scattered = Ray(rec.p, mixPDF.generate());
		scattered = Ray(rec.p, srec.PDF_ptr->generate(sampler));
		pdf = srec.PDF_ptr->value(scattered.direction());
	}

	// Recursively scatter rays
	return emitted + srec.attenuation * rec.mat->bsdf(r, rec, scattered)
		* dot(rec.normal, scattered.direction())
		* ray_colour(scattered, scene, arena, sampler) 
		* invpContinue / pdf;
}

struct RenderSettings {
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
		  threads(threads), tileSize(tileSize), outputPath(outputPath) {}

	RenderSettings(const RenderSettings& other) 
		: imageWidth(other.imageWidth), imageHeight(other.imageHeight), 
		  samplesPerPixel(other.samplesPerPixel), threads(other.threads),
		  tileSize(other.tileSize){}

	double aspectRatio() const { return imageWidth / (double)imageHeight; }
};

struct ScanlineBlock {
	const unsigned int blockID; 
	const RenderSettings renderSettings;
	Sampler sampler;
	MemoryArena arena;

	ScanlineBlock(unsigned int blockID, const RenderSettings& renderSettings)
		: blockID(blockID), renderSettings(renderSettings), arena(), 
		  sampler(blockID)
	{};

	void execute(const Scene& scene, const Tile& tile) 
	{
		for (unsigned int j = tile.top; 
			 j < tile.top + tile.height; j++) 
		{
			for (unsigned int i = tile.left; 
				 i < tile.left + tile.width; i++) 
			{
				Colour pixel_colour(0, 0, 0);
				for (unsigned int s = 0; s < renderSettings.samplesPerPixel; s++) {
					double u = (i + sampler.getDouble()) / (renderSettings.imageWidth - 1.0);
					double v = (j + sampler.getDouble()) / (renderSettings.imageHeight - 1.0);
					Ray r = scene.mainCam.get_ray(u, v, sampler);
					pixel_colour += ray_colour(r, scene, arena, sampler);
				}
				unsigned int tileIndex = (j - tile.top) * tile.width + i - tile.left;
				tile.colours[tileIndex] = pixel_colour / renderSettings.samplesPerPixel;
			}
		}
	}
};

class TilesQueue
{
public:
	TilesQueue() {}

	void addTile(std::shared_ptr<Tile> tile)
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		tiles.push_back(tile);
	}

	Tile* getTile() 
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		if (currentIndex < tiles.size())
			return tiles[currentIndex++].get();
		return nullptr;
	}
private:
	std::vector<std::shared_ptr<Tile>> tiles;
	int currentIndex;
	std::mutex queueMutex;
};

void executeBlock(std::shared_ptr<ScanlineBlock> block, Scene scene, 
			      std::shared_ptr<Film> film, 
				  std::shared_ptr<TilesQueue> tilesQueue) 
{
	Tile* tile = tilesQueue->getTile();
	while(tile != nullptr)
	{
		block->execute(scene, *tile);
		film->writeTile(*tile);
		tile = tilesQueue->getTile();
		block->arena.Reset();
	}
}

void execute(const RenderSettings& renderSettings, const Scene& scene) 
{
	Timer programTimer("execute");

	std::shared_ptr<Film> film = std::make_shared<Film>(
		renderSettings.imageWidth, renderSettings.imageHeight);

	unsigned int tileSize = renderSettings.tileSize;
	std::shared_ptr<TilesQueue> tilesQueue = std::make_shared<TilesQueue>();

	unsigned int remainingHeight = film->height;
	while (remainingHeight > 0)
	{
		unsigned int height = remainingHeight > tileSize ? tileSize : remainingHeight;
		unsigned int remainingWidth = film->width;
		while (remainingWidth > 0)
		{
			unsigned int width = remainingWidth > tileSize ? tileSize : remainingWidth;
			tilesQueue->addTile(
				std::make_shared<Tile>(
					film->height - remainingHeight, 
					film->width - remainingWidth, 
					width, height)
			);
			remainingWidth -= width;
		}
		remainingHeight -= height;
	}

	std::vector<std::thread> threads(renderSettings.threads);
	for (size_t i = 0; i < renderSettings.threads; ++i) {
		std::shared_ptr<ScanlineBlock> block 
			= std::make_shared<ScanlineBlock>(i, renderSettings);
		threads[i] = std::thread(executeBlock, block, scene, film, tilesQueue);
	}

	for (std::thread& thread : threads) {
		thread.join();
	}

	film->writeToFile(renderSettings.outputPath);
}

Camera defaultCamera(const RenderSettings& renderSettings)
{
	Point3 lookFrom(278, 288, -800);
	Point3 lookAt(278, 278, 0);
	double fov = 40;
	double aperture = 0.05;
	Vec3 vup(0, 1, 0);
	double focus_dist = (lookFrom - lookAt).length();
	double aspectRatio = renderSettings.aspectRatio();
	return Camera(lookFrom, lookAt, vup, fov, aspectRatio, 
				  aperture, focus_dist);
}

int main(int argc, char** argv) {
	argparse::ArgumentParser program("ray");

	program.add_argument("--width")
		.scan<'u', unsigned int>()
		.help("Image width")
		.required();
	program.add_argument("--height")
		.scan<'u', unsigned int>()
		.help("Image height")
		.required();
	program.add_argument("--spp")
		.scan<'u', unsigned int>()
		.help("Number of samples per pixel")
		.default_value(20u);
	program.add_argument("--threads")
		.scan<'u', unsigned int>()
		.help("Number of parallel threads to run. Default is the maximum available")
		.default_value(std::thread::hardware_concurrency());
	program.add_argument("--tilesize")
		.scan<'u', unsigned int>()
		.help("Tile size")
		.default_value(64u);
	program.add_argument("out")
		.help("Output path");

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program << std::endl;
		return 1;
	}

	const unsigned int width = program.get<unsigned int>("--width");
	const unsigned int height = program.get<unsigned int>("--height");
	const unsigned int spp = program.get<unsigned int>("--spp");
	const unsigned int threads = program.get<unsigned int>("--threads");
	const unsigned int tileSize = program.get<unsigned int>("--tilesize");
	const std::string out = program.get<std::string>("out");
	
	RenderSettings renderSettings(width, height, spp, threads, tileSize, out);
	execute(renderSettings, cornellBox(defaultCamera(renderSettings)));
	return 0;
}