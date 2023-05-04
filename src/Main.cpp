#include <iostream>
#include <chrono>
#include <thread>

#include "argparse/argparse.hpp"

#include "rtutils.h"

#include "AABB.h"
#include "Camera.h"
#include "Colour.h"
#include "PDF.h"
#include "memory.h"

#include "geom/AARect.h"
#include "geom/BVHNode.h"
#include "geom/Disk.h"
#include "geom/HittableList.h"
#include "geom/Mesh.h"
#include "geom/Sphere.h"
#include "geom/Transform.h"
#include "geom/Triangle.h"

#include "material/Material.h"
#include "material/Texture.h"

size_t MemoryArena::blockCount = 0;

// Recursive function for calculating intersections and colour
Colour ray_colour(const Ray& r, const Texture& background, 
			      const Hittable& world, const Hittable& lights,
				  MemoryArena& arena) {	
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
		return srec.attenuation * ray_colour(srec.specularRay, background, world, lights, arena)
			* dot(rec.normal, srec.specularRay.direction());
	}

	// Determine russian roulette termination
	//float maxChannel = fmax(fmax(srec.attenuation[0], srec.attenuation[1]), srec.attenuation[2]);
	//float pContinue = 1 - maxChannel;
	float pContinue = 1 - (srec.attenuation.length() / sqrt(3));
	pContinue = pContinue < 0 ? 0 : pContinue;
	float invpContinue = 1 / (1 - pContinue);
	float randomRussian = random_double();
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
		scattered = Ray(rec.p, srec.PDF_ptr->generate());
		pdf = srec.PDF_ptr->value(scattered.direction());
	}

	// Recursively scatter rays
	return emitted + srec.attenuation * rec.mat->bsdf(r, rec, scattered)
		* dot(rec.normal, scattered.direction())
		* ray_colour(scattered, background, world, lights, arena) * invpContinue / pdf;
}

// Cornell Box scene
HittableList cornellBox() {
	// Materials
	std::shared_ptr<Lambertian> red = std::make_shared<Lambertian>(Colour(.65, .05, .05));
	std::shared_ptr<Lambertian> white = std::make_shared<Lambertian>(Colour(.73, .73, .73));
	std::shared_ptr<Lambertian> green = std::make_shared<Lambertian>(Colour(.12, .45, .15));
	std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(Colour(12.0, 12.0, 12.0));
	std::shared_ptr<Dialectric> glass = std::make_shared<Dialectric>(1.5);
	std::shared_ptr<Metal> metal = std::make_shared<Metal>(Colour(0.9, 0.6, 0.1), 0.92);

	// Objects
	HittableList objects;
	objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));		// left wall
	objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));			// right wall
	objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));		// bottom wall
	objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));		// top wall
	objects.add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));		// back wall
	objects.add(std::make_shared<XZRect>(103, 453, 117, 442, 554, light));	// light
	objects.add(std::make_shared<Sphere>(Vec3(278, 278, 278), 150, metal));	// metal ball

	return objects;
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
	Colour* colours;
	size_t coloursCount;
	MemoryArena arena;

	ScanlineBlock(unsigned int blockID, unsigned int offset, 
				  unsigned int height, const RenderSettings& renderSettings) 
			: blockID(blockID), offset(offset), height(height), 
		      renderSettings(renderSettings), arena() {
		const unsigned int totalPixels = renderSettings.imageWidth * height;
		colours = new Colour[totalPixels];
		coloursCount = totalPixels;
	};

	~ScanlineBlock() {
		delete[] colours;
	}

	void execute(const Camera& cam, const Texture& sb_tex, const Hittable& world, const Hittable& lights) {
		for (unsigned int j = this->offset; j < this->offset + this->height; j++) {
			for (unsigned int i = 0; i < renderSettings.imageWidth; i++) {
				Colour pixel_colour(0, 0, 0);
				for (unsigned int s = 0; s < renderSettings.samplesPerPixel; s++) {
					double u = (i + random_double()) / (renderSettings.imageWidth - 1.0);
					double v = (j + random_double()) / (renderSettings.imageHeight - 1.0);
					Ray r = cam.get_ray(u, v);
					pixel_colour += ray_colour(r, sb_tex, world, lights, arena);
				}
				unsigned int pIndex = (j - this->offset) * renderSettings.imageWidth + i;
				colours[pIndex] = pixel_colour / renderSettings.samplesPerPixel;
			}
		}
	}
};

void executeBlock(std::shared_ptr<ScanlineBlock> block) {
	// FIXME: All of this scene creation can go in another class
	// Create camera
	Point3 lookFrom(278, 288, -800);
	Point3 lookAt(278, 278, 0);
	double fov = 40;
	double aperture = 0.05;
	Vec3 vup(0, 1, 0);
	double focus_dist = (lookFrom - lookAt).length();
	double aspectRatio = block->renderSettings.aspectRatio();
	Camera cam(lookFrom, lookAt, vup, fov, aspectRatio, aperture, focus_dist);

	// Create skybox
	SolidColour sb_tex(0.1, 0.1, 0.1);

	// Create world
	std::shared_ptr<Lambertian> red = std::make_shared<Lambertian>(Colour(.8, .05, .05));
	Sphere world(Vec3(278, 278, 278), 150, red);

	// Create lights
	std::shared_ptr<DiffuseLight> lightmat = std::make_shared<DiffuseLight>(Colour(1, 1, 1));
	XZRect lights(163, 393, 177, 382, 554, lightmat);

	// Execute
	std::cerr << "\nBlock " << block->blockID << " started";
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	block->execute(cam, sb_tex, world, lights);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cerr << "\nBlock " << block->blockID << " finished: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "s\n" << std::endl;
}

void execute(const RenderSettings& renderSettings) {
	// Start clock
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	
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
		threads[i] = std::thread(executeBlock, blocks[i]);
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
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//std::cout << "# Time Taken: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "s\n";
	std::cerr << "\nTotal time elapsed = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "s\n" << std::endl;
	std::cerr << "\nDone.\n";
	std::cerr << "\n";
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
	execute(renderSettings);
	return 0;
}