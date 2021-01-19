#include "rtutils.h"

#include "Camera.h"
#include "Colour.h"
#include "Sphere.h"
#include "AARect.h"
#include "AABB.h"
#include "BVHNode.h"
#include "Disk.h"
#include "Triangle.h"
#include "HittableList.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dialectric.h"
#include "DiffuseLight.h"
#include <iostream>
#include <chrono>

// Recursive function for calculating intersections and colour
Colour ray_colour(const Ray& r, std::shared_ptr<Texture> background, const Hittable& world, int depth) {
	// Base case for recursion (add no more colour)
	if (depth <= 0) {
		return Colour(0, 0, 0);
	}

	// If the ray hits nothing, return the skybox colour
	hit_record rec;
	if (!world.hit(r, 0.001, infinity, rec)) {
		// Compute u,v of hit
		double u, v;
		Sphere::get_sphere_uv(unit_vector(r.direction()), u, v);
		return background->value(u,v,r.origin());
	}

	Ray scattered;
	Colour attenuation;
	Colour emitted = rec.mat->emitted(0,0,Vec3(0,0,0));	// needs to change when u,v coordinates is implemented
	if (!rec.mat->scatter(r, rec, attenuation, scattered))
		return emitted;

	// Recursively scatter rays
	return emitted + attenuation * ray_colour(scattered, background, world, depth - 1);
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
	objects.add(std::make_shared<XZRect>(163, 393, 177, 382, 554, light));
	objects.add(std::make_shared<Sphere>(Vec3(278, 278, 278), 150, image_mat));
	objects.add(std::make_shared<Sphere>(Vec3(278, 278, 278), 170, glass));

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
	// Materials
	std::shared_ptr<Lambertian> red = std::make_shared<Lambertian>(Colour(.8, .05, .05));
	std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(Colour(5, 5, 5));

	// Objects
	HittableList objects;
	objects.add(std::make_shared<XZRect>(-1, 1, -1, 1, 2, light));
	objects.add(std::make_shared<XZRect>(-1, 1, -1, 1, -2, red));

	// Triangle
	auto v0 = std::make_shared<vertex_triangle>();
	auto v1 = std::make_shared<vertex_triangle>();
	auto v2 = std::make_shared<vertex_triangle>();
	v0->pos = Vec3(-1.0,-1.0,0.0);
	v0->u = 0;
	v0->v = 0;
	v1->pos = Vec3(1.0, -1.0, 0.0);
	v1->u = 1;
	v1->v = 0;
	v2->pos = Vec3(-1.0, 1.0, -1.0);
	v2->u = 0;
	v2->v = 1;
	objects.add(std::make_shared<Triangle>(v0, v1, v2, red));

	return objects;
}

int main() {

	// Start clock
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	
	// Image properties

	const double aspect_ratio = 1.0;
	const int image_width = 800;
	const int image_height = (int)(image_width / aspect_ratio);
	const int samples_per_pixel = 0;
	const int max_depth = 8;

	// Camera

	Point3 lookFrom;
	Point3 lookAt;
	double fov = 70;
	double aperture = 0.0;

	// World properties
	HittableList world;
	std::shared_ptr<Texture> sb_tex;

	switch (2) {
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
			lookFrom = Point3(0, 0, 5);
			lookAt = Point3(0, 0, 0);
			world = testScene();
			sb_tex = std::make_shared<SolidColour>(0.4, 0.4, 0.4);
			break;
		// Sphere setup
		default:
		case 3:
			lookFrom = Point3(-100, 500, -100);
			lookAt = Point3(278, 278, 278);
			world = spheres();
			sb_tex = std::make_shared<SolidColour>(0.0, 0.0, 0.0);
			break;
	}

	// Camera

	Vec3 vup(0, 1, 0);
	double focus_dist = (lookFrom - lookAt).length();
	Camera cam(lookFrom, lookAt, vup, fov, aspect_ratio, aperture, focus_dist);

	// Render

	// Set up the ppm file headers
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	// Point this to an object if you want to test AABB
	std::shared_ptr<Hittable> test_object = 0;

	// Generate image
	// Note how j goes in reverse order - this is so scans go from bot to top
	for (int j = image_height-1; j >= 0; j--) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; i++) {
			// Debug AABB
			if (test_object != NULL) {
				double u = (i) / (image_width - 1.0);
				double v = (j) / (image_height - 1.0);
				Ray r = cam.get_ray(u, v);
				Colour pixel_colour = aabb_test(r, test_object);
				write_colour(std::cout, pixel_colour);
				continue;
			}

			// Don't randomize ray direction if samples = 0
			if (samples_per_pixel == 0) {
				double u = (i) / (image_width - 1.0);
				double v = (j) / (image_height - 1.0);
				Ray r = cam.get_ray(u, v);
				Colour pixel_colour = ray_colour(r, sb_tex, world, max_depth);
				write_colour(std::cout, pixel_colour);
			}
			else {
				Colour pixel_colour(0, 0, 0);
				for (int s = 0; s < samples_per_pixel; s++) {
					double u = (i + random_double()) / (image_width - 1.0);
					double v = (j + random_double()) / (image_height - 1.0);
					Ray r = cam.get_ray(u, v);
					pixel_colour += ray_colour(r, sb_tex, world, max_depth);
				}
				write_colour(std::cout, pixel_colour / samples_per_pixel);
			}
		}
	}

	// End Clock
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cerr << "\nTotal time elapsed = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "s\n" << std::endl;
	std::cerr << "\nDone.\n";
}