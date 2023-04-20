#include <iostream>
#include <chrono>
#include <thread>

#include "argparse/argparse.hpp"

#include "rtutils.h"

#include "AABB.h"
#include "Camera.h"
#include "Colour.h"
#include "PDF.h"

#include "geom/AARect.h"
#include "geom/BVHNode.h"
#include "geom/Disk.h"
#include "geom/HittableList.h"
#include "geom/Mesh.h"
#include "geom/Sphere.h"
#include "geom/Transform.h"
#include "geom/Triangle.h"

#include "material/Dialectric.h"
#include "material/DiffuseLight.h"
#include "material/Lambertian.h"
#include "material/Metal.h"

// Debugging a single thread will be easier than debugging multiple
#if DEBUG_MODE == 0
#define THREAD_COUNT 1
#else
#define THREAD_COUNT 1
#endif

// Recursive function for calculating intersections and colour
Colour ray_colour(const Ray& r, std::shared_ptr<Texture> background, const Hittable& world, std::shared_ptr<Hittable> lights) {	
	// If the ray hits nothing, return the skybox colour
	hit_record rec;
	if (!world.hit(r, 0.001, infinity, rec)) {
		// Compute u,v of hit
		double u, v;
		Sphere::get_sphere_uv(unit_vector(r.direction()), u, v);
		return background->value(u,v,rec);
	}

	Colour emitted = rec.mat->emitted(0,0,Vec3(0,0,0));	// needs to change when u,v coordinates is implemented
	scatter_record srec;
	if (!rec.mat->scatter(r, rec, srec))
		return emitted;

	// If the generated ray is specular, then we do not need to sample directions
	// (this is because the specular only has one possible scattering ray)
	if (srec.isSpecular) {
		return srec.attenuation * ray_colour(srec.specularRay, background, world, lights)
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
		std::shared_ptr<HittablePDF> lightPDF = std::make_shared<HittablePDF>(lights, rec.p);
		MixturePDF mixPDF = MixturePDF(srec.PDF_ptr, lightPDF);
		scattered = Ray(rec.p, mixPDF.generate());
		pdf = mixPDF.value(scattered.direction());
	}

	// Recursively scatter rays
	return emitted + srec.attenuation * rec.mat->bsdf(r, rec, scattered)
		* dot(rec.normal, scattered.direction())
		* ray_colour(scattered, background, world, lights) * invpContinue / pdf;
}

// Renders an objects AABB for debug purposes
Colour aabb_test(const Ray& r, std::shared_ptr<Hittable> object) {
	AABB aabb;
	object->bounding_box(0,0,aabb);
	if (aabb.hit(r, 0.001, infinity)) {
		return Colour(1.0, 1.0, 1.0);
	}
	return Colour(0.0, 0.0, 0.0);
}

// Cornell Box scene
HittableList cornellBox() {
	// Textures
	std::shared_ptr<ImageTexture> image_texture = std::make_shared<ImageTexture>("images/globe2.jpg");

	// Materials
	std::shared_ptr<Lambertian> red = std::make_shared<Lambertian>(Colour(.65, .05, .05));
	std::shared_ptr<Lambertian> white = std::make_shared<Lambertian>(Colour(.73, .73, .73));
	std::shared_ptr<Lambertian> green = std::make_shared<Lambertian>(Colour(.12, .45, .15));
	std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(Colour(12.0, 12.0, 12.0));
	std::shared_ptr<Dialectric> glass = std::make_shared<Dialectric>(1.5);
	std::shared_ptr<Lambertian> image_mat = std::make_shared<Lambertian>(image_texture);
	std::shared_ptr<Metal> metal = std::make_shared<Metal>(Colour(0.9, 0.6, 0.1), 0.92);

	//std::shared_ptr<Hittable> mesh = std::make_shared<Mesh>("models/greek.obj", 0.5, true, white);
	//std::shared_ptr<Hittable> sp = std::make_shared<Sphere>(Vec3(0, 0, 0), 1, image_mat);


	// Objects
	HittableList objects;
	objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));		// left wall
	objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));			// right wall
	objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));		// bottom wall
	objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));		// top wall
	objects.add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));		// back wall
	objects.add(std::make_shared<XZRect>(103, 453, 117, 442, 554, light));	// light
	objects.add(std::make_shared<Sphere>(Vec3(278, 278, 278), 150, metal));	// metal ball
	//objects.add(mesh);
	//objects.add(std::make_shared<Transform>(mesh, Vec3(265, 185, 278), Vec3(0, 180, 0), Vec3(200.0, 200.0, 200.0)));
	//objects.add(std::make_shared<Sphere>(Vec3(278, 278, 278), 170, glass));

	return objects;
}

// Scene for test purposes 
HittableList spheres() {
	// Materials
	std::shared_ptr<Lambertian> red = std::make_shared<Lambertian>(Colour(.65, .05, .05));
	std::shared_ptr<Lambertian> white = std::make_shared<Lambertian>(Colour(.73, .73, .73));
	std::shared_ptr<Lambertian> green = std::make_shared<Lambertian>(Colour(.12, .45, .15));
	std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(Colour(5, 5, 5));

	// Objects
	HittableList objects;
	objects.add(std::make_shared<XZRect>(163, 393, 177, 382, 554, light));
	objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 555, white));
	objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));
	objects.add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));

	// Spheres
	HittableList red_spheres;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			for (int k = 0; k < 10; k++) {
				red_spheres.add(std::make_shared<Sphere>(Vec3(148 + 24 * i + random_double(0,4), 148 + 24 * j + random_double(0, 4), 148 + 24 * k + random_double(0, 4)), 10 + random_double(0, 6), red));
			}
		}
	}
	objects.add(std::make_shared<BVHNode>(red_spheres, 0, 0));
	
	return objects;
}

HittableList testScene() {
	// Texture
	std::shared_ptr<ImageTexture> image_texture = std::make_shared<ImageTexture>("images/snowglobe.png");

	// Materials
	std::shared_ptr<Lambertian> red = std::make_shared<Lambertian>(Colour(.8, .05, .05));
	std::shared_ptr<Metal> metal = std::make_shared<Metal>(Colour(.4, .4, .4), 0.5);
	std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(Colour(5, 5, 5));
	std::shared_ptr<Dialectric> glass = std::make_shared<Dialectric>(1.1);
	std::shared_ptr<Lambertian> white = std::make_shared<Lambertian>(Colour(.72, .77, .77));
	std::shared_ptr<Lambertian> image_mat = std::make_shared<Lambertian>(image_texture);

	std::shared_ptr<NormalTexture> normal_texture = std::make_shared<NormalTexture>();
	std::shared_ptr<Lambertian> normal_mat = std::make_shared<Lambertian>(normal_texture);

	std::shared_ptr<CheckerTexture> checker = std::make_shared<CheckerTexture>(Colour(1,1,1), Colour(0.1,0.1,0.1));
	std::shared_ptr<Lambertian> checker_mat = std::make_shared<Lambertian>(checker);
	
	// Objects
	HittableList objects;
	//objects.add(std::make_shared<XZRect>(-3, 3, -3, 3, 6, light));
	//std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>("models/greek.obj", 0.5, false, red);
	std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(Vec3(0,0,0), 0.5, normal_mat);
	objects.add(std::make_shared<Transform>(sphere, Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(1.0, 0.5, 1.0)));
	objects.add(std::make_shared<Transform>(sphere, Vec3(0, 0.4, 0), Vec3(0, 0, 0), Vec3(1.0, 0.5, 1.0)));
	objects.add(std::make_shared<Transform>(sphere, Vec3(0, 0.8, 0), Vec3(0, 0, 0), Vec3(1.0, 0.5, 1.0)));
	//objects.add(std::make_shared<Transform>(sphere, Vec3(1, 0, 0), Vec3(0, 0, 0), Vec3(1.0, 1.0, 1.0)));
	//objects.add(std::make_shared<Transform>(mesh, Vec3(0.5, 0.2, 0), Vec3(-20, 10, 0), Vec3(1.0, 1.0, 1.0)));
	// objects.add(sphere);

	// Test ccw triangle intersections to find where we are going wrong
	/*auto v0 = std::make_shared<vertex_triangle>();
	auto v1 = std::make_shared<vertex_triangle>();
	auto v2 = std::make_shared<vertex_triangle>();

	v0->pos = Vec3(0,0,0);
	v1->pos = Vec3(1,0,0);
	v2->pos = Vec3(0,1,0);
	objects.add(std::make_shared<Triangle>(v0, v2, v1, red, false));*/

	return objects;
}

HittableList testSky() {
	// Materials
	std::shared_ptr<DiffuseLight> red = std::make_shared<DiffuseLight>(Colour(4.95, .05, .05));
	std::shared_ptr<DiffuseLight> green = std::make_shared<DiffuseLight>(Colour(.05, 4.95, .05));
	std::shared_ptr<DiffuseLight> blue = std::make_shared<DiffuseLight>(Colour(.05, .05, 4.95));
	std::shared_ptr<Metal> metal = std::make_shared<Metal>(Colour(.4, .4, .4), 0.0);
	std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(Colour(3, 3, 3));
	std::shared_ptr<Lambertian> white = std::make_shared<Lambertian>(Colour(1, 1, 1));

	// Object instances
	std::shared_ptr<Hittable> obj = std::make_shared<Mesh>("models/hood_greek.obj", 6, true, light);
	std::shared_ptr<Hittable> obj2 = std::make_shared<Mesh>("models/greek.obj", 3, true, metal);

	// Objects
	HittableList objects;
	//objects.add(std::make_shared<XZRect>(-3, 3, -3, 3, 6, light));
	/*objects.add(std::make_shared<Translate>(obj, Vec3(0, 1, 25)));
	objects.add(std::make_shared<Translate>(obj2, Vec3(0, -0.5, 0)));*/
	objects.add(std::make_shared<XZRect>(-2000, 2000, -2000, 2000, -0.5, white));

	// RGB Spheres
	std::shared_ptr<Hittable> s1 = std::make_shared<Sphere>(Vec3(-5, 5, 15), 2.5, red);
	std::shared_ptr<Hittable> s2 = std::make_shared<Sphere>(Vec3(5, 15, 10), 2, blue);
	std::shared_ptr<Hittable> s3 = std::make_shared<Sphere>(Vec3(10, 0, 10), 2, green);
	objects.add(s1);
	objects.add(s2);
	objects.add(s3);

	return objects;
}

void threadTest(int offset, int height, int image_width, int image_height, int samples_per_pixel, const Camera& cam,
		std::shared_ptr<Texture> sb_tex, const Hittable& world, std::shared_ptr<Hittable> lights, std::vector<Colour>& output, int& progress) {
	for (int j = offset + height - 1; j >= offset; j--) {
		//std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		progress = j - offset;
		for (int i = 0; i < image_width; i++) {
			Colour pixel_colour(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; s++) {
				double u = (i + random_double()) / (image_width - 1.0);
				double v = (j + random_double()) / (image_height - 1.0);
				Ray r = cam.get_ray(u, v);
				pixel_colour += ray_colour(r, sb_tex, world, lights);
			}
			//write_colour(std::cout, pixel_colour / samples_per_pixel);
			output.push_back(pixel_colour / samples_per_pixel);
		}
	}
}

void printProgressOfThreads(int* progresses, int count) {
	while (true) {
		// Determine whether or not we should break after
		// we print the next statement
		bool cancel = true;
		for (int i = 0; i < count; i++) {
			if (progresses[i] > 0) {
				cancel = false;
				break;
			}
		}
		
		// Print progress
		std::cerr << "\r";
		if (progresses[0] == 0)
			std::cerr << "DONE";
		else
			std::cerr << progresses[0];

		for (int i = 1; i < count; i++) {
			if (progresses[i] == 0)
				std::cerr << "..." << "DONE";
			else
				std::cerr << "..." << progresses[i];
		}
		std::cerr << "            " << std::flush;

		// Break if we need
		if (cancel)
			break;
	}
}

struct RenderSettings {
	int imageWidth;
	int imageHeight;
	int samplesPerPixel;
	
	RenderSettings(int w, int h, int spp) 
		: imageWidth(w), imageHeight(h), samplesPerPixel(spp) {}

	double aspectRatio() const { return imageWidth / (double)imageHeight; }
};

void execute(const RenderSettings& renderSettings) {
	// Start clock
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	
	// Camera
	Point3 lookFrom;
	Point3 lookAt;
	double fov = 48.5;
	double aperture = 0;

	// World properties
	HittableList world;
	std::shared_ptr<Texture> sb_tex;
	//objects.add(std::make_shared<XZRect>);
	std::shared_ptr<DiffuseLight> lightmat = std::make_shared<DiffuseLight>(Colour(1, 1, 1));
	std::shared_ptr<XZRect> lights = std::make_shared<XZRect>(163, 393, 177, 382, 554, lightmat);

	switch (1) {
		// Cornell box setup
		case 1:
			lookFrom = Point3(278, 278, -800);
			lookAt = Point3(278, 278, 0);
			fov = 40;
			aperture = 0.05;
			world = cornellBox();
			sb_tex = std::make_shared<SolidColour>(0.0,0.0,0.0);
			break;
		// Test setup
		case 2:
			lookFrom = Point3(0, 1, 2.7);
			lookAt = Point3(0, 0.5, 0);
			world = testScene();
			sb_tex = std::make_shared<SolidColour>(0.5, 0.5, 0.5);
			break;
		case 3:
			lookFrom = Point3(15, 8, 15);
			lookAt = Point3(0, 3, 0);
			world = testSky();
			//sb_tex = std::make_shared<ImageTexture>("images/small_hangar.hdr");
			sb_tex = std::make_shared<SolidColour>(0., 0., 0.);
			break;
		// Sphere setup
		default:
		case 4:
			lookFrom = Point3(-100, 500, -100);
			lookAt = Point3(278, 278, 278);
			world = spheres();
			sb_tex = std::make_shared<SolidColour>(0.0, 0.0, 0.0);
			break;
	}

	// Camera

	Vec3 vup(0, 1, 0);
	double focus_dist = (lookFrom - lookAt).length();
	Camera cam(lookFrom, lookAt, vup, fov, renderSettings.aspectRatio(), aperture, focus_dist);

	// Render

	// Set up the ppm file headers
	std::cout << "P3\n" << renderSettings.imageWidth << ' ' << renderSettings.imageHeight << "\n255\n";
	
	// Place some metadata in the ppm
	std::cout << "# Samples Per Pixel: " << renderSettings.samplesPerPixel << "\n";

	// Point this to an object if you want to test AABB
	std::shared_ptr<Hittable> test_object = 0;

	std::thread threads[THREAD_COUNT];
	std::vector<Colour> scanlineParts[THREAD_COUNT];
	int threadProgresses[THREAD_COUNT];
	int scanlineHeight = renderSettings.imageHeight / THREAD_COUNT;
	
	// Spawn threads
	for (int i = 0; i < THREAD_COUNT; i++) {
		// The last thread may have less scanlines due to some division errors
		// so we need to account for that here
		if (i == THREAD_COUNT - 1 && scanlineHeight * (i + 1) != renderSettings.imageWidth) {
			threads[i] = std::thread(threadTest, scanlineHeight * i, renderSettings.imageHeight - scanlineHeight * i, renderSettings.imageWidth, renderSettings.imageHeight,
				renderSettings.samplesPerPixel, std::ref(cam), sb_tex, std::ref(world), lights, std::ref(scanlineParts[i]), std::ref(threadProgresses[i]));
			continue;
		}

		// Spawn thread normally
		threads[i] = std::thread(threadTest, scanlineHeight * i, scanlineHeight, renderSettings.imageWidth, renderSettings.imageHeight,
			renderSettings.samplesPerPixel, std::ref(cam), sb_tex, std::ref(world), lights, std::ref(scanlineParts[i]), std::ref(threadProgresses[i]));
	}

	// Spawn progress thread (for logging progress)
	std::thread progressThread(printProgressOfThreads, threadProgresses, THREAD_COUNT);

	// Join threads
	for (int i = 0; i < THREAD_COUNT; i++) {
		threads[i].join();
	}

	// Wait til progress thread is done (it should be at this point)
	progressThread.join();

	// Combine all the scanlines into one image
	std::cerr << "\nWriting image to file\n";
	for (int j = THREAD_COUNT-1; j >= 0; j--) {
		for (int i = 0; i < scanlineHeight * renderSettings.imageWidth; i++) {
			write_colour(std::cout, scanlineParts[j][i]);
		}
	}

	// End Clock
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//std::cout << "# Time Taken: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "s\n";
	std::cerr << "\nTotal time elapsed = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "s\n" << std::endl;
	std::cerr << "\nDone.\n";
	std::cerr << "\n";
}

int main(int argc, char** argv) {
	argparse::ArgumentParser program("ray");

	program.add_argument("--width")
		.scan<'d', int>()
		.help("Image width")
		.required();
	program.add_argument("--height")
		.scan<'d', int>()
		.help("Image height")
		.required();
	program.add_argument("--spp")
		.scan<'d', int>()
		.help("Number of samples per pixel")
		.default_value(20);

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program << std::endl;
		return 1;
	}

	const int width = program.get<int>("--width");
	const int height = program.get<int>("--height");
	const int spp = program.get<int>("--spp");
	
	RenderSettings renderSettings(width, height, spp);
	execute(renderSettings);
	return 0;
}