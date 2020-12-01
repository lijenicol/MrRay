#include "rtutils.h"

#include "Camera.h"
#include "Colour.h"
#include "Sphere.h"
#include "AARect.h"
#include "HittableList.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dialectric.h"
#include "DiffuseLight.h"
#include <iostream>

Colour ray_colour(const Ray& r, const Colour& background, const Hittable& world, int depth) {
	// Base case for recursion (add no more colour)
	if (depth <= 0) {
		return Colour(0, 0, 0);
	}

	// If the ray hits nothing, return the background colour
	hit_record rec;
	if (!world.hit(r, 0.001, infinity, rec))
		return background;

	Ray scattered;
	Colour attenuation;
	Colour emitted = rec.mat->emitted(0,0,Vec3(0,0,0));	// needs to change when u,v coordinates is implemented
	if (!rec.mat->scatter(r, rec, attenuation, scattered))
		return emitted;

	// Recursively scatter rays
	return emitted + attenuation * ray_colour(scattered, background, world, depth - 1);
}

HittableList cornellBox() {
	// Textures
	std::shared_ptr<ImageTexture> image_texture = std::make_shared<ImageTexture>("images/globe2.jpg");

	// Materials
	std::shared_ptr<Lambertian> red = std::make_shared<Lambertian>(Colour(.65, .05, .05));
	std::shared_ptr<Lambertian> white = std::make_shared<Lambertian>(Colour(.73, .73, .73));
	std::shared_ptr<Lambertian> green = std::make_shared<Lambertian>(Colour(.12, .45, .15));
	std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(Colour(5, 5, 5));
	std::shared_ptr<Dialectric> glass = std::make_shared<Dialectric>(1.5);
	std::shared_ptr<Lambertian> image_mat = std::make_shared<Lambertian>(image_texture);

	// Objects
	HittableList objects;
	objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));
	objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));
	objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));
	objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));
	objects.add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));
	//objects.add(std::make_shared<XZRect>(213, 343, 227, 332, 554, light));
	objects.add(std::make_shared<XZRect>(163, 393, 177, 382, 554, light));
	objects.add(std::make_shared<Sphere>(Vec3(278, 278, 278), 150, image_mat));
	objects.add(std::make_shared<Sphere>(Vec3(278, 278, 278), 170, glass));

	return objects;
}

HittableList spheres() {
	// Textures
	std::shared_ptr<CheckerTexture> ct = std::make_shared<CheckerTexture>(Colour(0.8,0.1,0.1),Colour(1,1,1));
	std::shared_ptr<ImageTexture> image_texture = std::make_shared<ImageTexture>("images/globe2.jpg");

	// Materials
	std::shared_ptr<Lambertian> red = std::make_shared<Lambertian>(Colour(.65, .05, .05));
	std::shared_ptr<Lambertian> green = std::make_shared<Lambertian>(Colour(.12, .45, .15));
	std::shared_ptr<Lambertian> white = std::make_shared<Lambertian>(Colour(.73, .73, .73));
	std::shared_ptr<Lambertian> black = std::make_shared<Lambertian>(Colour(.08, .08, .08));
	std::shared_ptr<Lambertian> checkered = std::make_shared<Lambertian>(ct);
	std::shared_ptr<Lambertian> image_mat = std::make_shared<Lambertian>(image_texture);

	// Objects
	HittableList objects;
	objects.add(std::make_shared<Sphere>(Vec3(0, 0, 0), 5, image_mat));
	//objects.add(std::make_shared<XZRect>(-6,6,-6,6,0, image_mat));
	//objects.add(std::make_shared<Sphere>(Vec3(-6, 0, 0), 1, green));
	//objects.add(std::make_shared<Sphere>(Vec3(6, 0, 0), 1, red));
	//objects.add(std::make_shared<Sphere>(Vec3(0, -100, 0), 99, black));
	return objects;
}

int main() {
	
	// Image properties

	const double aspect_ratio = 1.0;
	const int image_width = 700;
	const int image_height = (int)(image_width / aspect_ratio);
	const int samples_per_pixel = 100;
	const int max_depth = 50;

	// Camera

	Point3 lookFrom;
	Point3 lookAt;
	double fov = 70;
	double aperture = 0.0;

	// World properties
	HittableList world;
	Colour background;

	switch (2) {
		// Cornell box setup
		case 1:
			lookFrom = Point3(278, 278, -800);
			lookAt = Point3(278, 278, 0);
			fov = 40;
			aperture = 0.05;
			background = Colour(0, 0, 0);
			world = cornellBox();
			break;
		// Sphere setup
		default:
		case 2:
			lookFrom = Point3(-10, 10, -10);
			lookAt = Point3(0, 0, 0);
			background = Colour(0.8, 0.8, 0.8);
			world = spheres();
			break;
	}

	// Camera

	Vec3 vup(0, 1, 0);
	double focus_dist = (lookFrom - lookAt).length();
	Camera cam(lookFrom, lookAt, vup, fov, aspect_ratio, aperture, focus_dist);

	// Render

	// Set up the ppm file headers
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	// Generate image
	// Note how j goes in reverse order - this is so scans go from bot to top
	for (int j = image_height-1; j >= 0; j--) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; i++) {
			// Don't randomize ray direction if samples = 0
			if (samples_per_pixel == 0) {
				double u = (i) / (image_width - 1.0);
				double v = (j) / (image_height - 1.0);
				Ray r = cam.get_ray(u, v);
				Colour pixel_colour = ray_colour(r, background, world, max_depth);
				write_colour(std::cout, pixel_colour);
			}
			else {
				Colour pixel_colour(0, 0, 0);
				for (int s = 0; s < samples_per_pixel; s++) {
					double u = (i + random_double()) / (image_width - 1.0);
					double v = (j + random_double()) / (image_height - 1.0);
					Ray r = cam.get_ray(u, v);
					pixel_colour += ray_colour(r, background, world, max_depth);
				}
				write_colour(std::cout, pixel_colour / samples_per_pixel);
			}
		}
	}

	std::cerr << "\nDone.\n";
}