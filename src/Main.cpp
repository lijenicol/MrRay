#include <iostream>
#include <chrono>
#include <thread>

#include "argparse/argparse.hpp"

#include "rtutils.h"

#include "Camera.h"
#include "Colour.h"
#include "PDF.h"
#include "memory.h"
#include "sampler.h"
#include "scene.h"
#include "timer.h"

#include "geom/Sphere.h"

#include "material/Material.h"
#include "material/Texture.h"

size_t MemoryArena::blockCount = 0;

// Recursive function for calculating intersections and colour
Colour ray_colour(const Ray& r, const Texture& background, 
			      const Hittable& world, Hittable* lights,
				  MemoryArena& arena, Sampler& sampler) {	
	// If the ray hits nothing, return the skybox colour
	hit_record rec;
	if (!world.hit(r, 0.001, infinity, rec)) {
		// Compute u,v of hit
		double u, v;
		Sphere::get_sphere_uv(unit_vector(r.direction()), u, v);
		return background.value(u,v,rec);
	}

	Colour emitted = rec.mat->emitted(0,0,Vec3(0,0,0));	// needs to change when u,v coordinates is implemented
	scatter_record srec;
	if (!rec.mat->scatter(r, rec, srec, arena))
		return emitted;

	// If the generated ray is specular, then we do not need to sample directions
	// (this is because the specular only has one possible scattering ray)
	if (srec.isSpecular) {
		return srec.attenuation * ray_colour(srec.specularRay, background, world, lights, arena, sampler)
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
		* ray_colour(scattered, background, world, lights, arena, sampler) * invpContinue / pdf;
}

struct RenderSettings {
	unsigned int imageWidth;
	unsigned int imageHeight;
	unsigned int samplesPerPixel;
	unsigned int threads;
	
	RenderSettings(unsigned int w, unsigned int h, unsigned int spp, 
				   unsigned int threads) 
		: imageWidth(w), imageHeight(h), samplesPerPixel(spp), 
		  threads(threads) {}

	RenderSettings(const RenderSettings& other) 
		: imageWidth(other.imageWidth), imageHeight(other.imageHeight), 
		  samplesPerPixel(other.samplesPerPixel), threads(other.threads) {}

	double aspectRatio() const { return imageWidth / (double)imageHeight; }
};

struct ScanlineBlock {
	const unsigned int blockID; 
	const unsigned int offset;
	const unsigned int height;
	const RenderSettings renderSettings;
	Sampler sampler;
	Colour* colours;
	size_t coloursCount;
	MemoryArena arena;

	ScanlineBlock(unsigned int blockID, unsigned int offset, 
				  unsigned int height, const RenderSettings& renderSettings) 
			: blockID(blockID), offset(offset), height(height), 
		      renderSettings(renderSettings), arena(), sampler(blockID) {
		const unsigned int totalPixels = renderSettings.imageWidth * height;
		colours = new Colour[totalPixels];
		coloursCount = totalPixels;
	};

	~ScanlineBlock() {
		delete[] colours;
	}

	void execute(const Camera& cam, const Texture& sb_tex, const Hittable& world, Hittable* lights) {
		for (unsigned int j = this->offset; j < this->offset + this->height; j++) {
			for (unsigned int i = 0; i < renderSettings.imageWidth; i++) {
				Colour pixel_colour(0, 0, 0);
				for (unsigned int s = 0; s < renderSettings.samplesPerPixel; s++) {
					double u = (i + sampler.getDouble()) / (renderSettings.imageWidth - 1.0);
					double v = (j + sampler.getDouble()) / (renderSettings.imageHeight - 1.0);
					Ray r = cam.get_ray(u, v, sampler);
					pixel_colour += ray_colour(r, sb_tex, world, lights, arena, sampler);
				}
				unsigned int pIndex = (j - this->offset) * renderSettings.imageWidth + i;
				colours[pIndex] = pixel_colour / renderSettings.samplesPerPixel;
			}
		}
	}
};

void executeBlock(std::shared_ptr<ScanlineBlock> block, Scene scene) 
{
	// Execute
	block->execute(
		scene.mainCam, *(scene.skyboxTexture.get()), *(scene.world.get()), 
		nullptr);
}

void execute(const RenderSettings& renderSettings, const Scene& scene) 
{
	Timer programTimer("execute");

	// Set up the ppm file headers
	std::cout << "P3\n" << renderSettings.imageWidth << ' ' << renderSettings.imageHeight << "\n255\n";
	
	// Place some metadata in the ppm
	std::cout << "# Samples Per Pixel: " << renderSettings.samplesPerPixel << "\n";

	unsigned int scanlineHeight = renderSettings.imageHeight / renderSettings.threads;
	unsigned int remainingLines = renderSettings.imageHeight;

	// Create scanline blocks
	std::vector<std::shared_ptr<ScanlineBlock>> blocks(renderSettings.threads);
	for (size_t i = 0; i < renderSettings.threads - 1; ++i) {
		blocks[i] = std::make_shared<ScanlineBlock>(
			i, i * scanlineHeight, scanlineHeight, renderSettings);
		remainingLines -= scanlineHeight;
	}
	size_t i =  renderSettings.threads - 1;
	blocks[i] = std::make_shared<ScanlineBlock>(
		i, i * scanlineHeight, remainingLines, renderSettings);

	std::vector<std::thread> threads(renderSettings.threads);
	for (size_t i = 0; i < renderSettings.threads; ++i) {
		threads[i] = std::thread(executeBlock, blocks[i], scene);
	}

	for (std::thread& thread : threads) {
		thread.join();
	}

	for (const std::shared_ptr<ScanlineBlock>& block : blocks) {
		for (size_t i = 0; i < block->coloursCount; i++) {
			write_colour(std::cout, block->colours[i]);
		}
	}
	
	// End Clock
	std::cerr << "Block count: " << MemoryArena::blockCount << std::endl;
	std::cerr << "\nDone.\n";
	std::cerr << "\n";
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
	
	RenderSettings renderSettings(width, height, spp, threads);
	execute(renderSettings, cornellBox(defaultCamera(renderSettings)));
	return 0;
}