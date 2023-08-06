#include <iostream>
#include <thread>

#include "argparse/argparse.hpp"

#include "mrRay/camera.h"
#include "mrRay/renderEngine.h"
#include "mrRay/timer.h"

#include "mrRay/geom/aaRect.h"
#include "mrRay/geom/sphere.h"

#include "mrRay/material/material.h"

MR_RAY_NAMESPACE_USING_DIRECTIVE

/// Returns the cornell box scene, set up with a camera
std::shared_ptr<Scene> cornellBox(const RenderSettings& renderSettings)
{
    std::shared_ptr<Lambertian> red = std::make_shared<Lambertian>(Colour(.65, .05, .05));
    std::shared_ptr<Lambertian> white = std::make_shared<Lambertian>(Colour(.73, .73, .73));
    std::shared_ptr<Lambertian> green = std::make_shared<Lambertian>(Colour(.12, .45, .15));
    std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(Colour(12.0, 12.0, 12.0));
    std::shared_ptr<Metal> metal = std::make_shared<Metal>(Colour(0.9, 0.6, 0.1), 0.92);

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    scene->addHittable(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));
    scene->addHittable(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));
    scene->addHittable(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));
    scene->addHittable(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));
    scene->addHittable(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));
    scene->addHittable(std::make_shared<XZRect>(103, 453, 117, 442, 554, light));
    scene->addHittable(std::make_shared<Sphere>(Vec3(278, 278, 278), 150, white));

    std::shared_ptr<Camera> mainCam = std::make_shared<Camera>(
        Point3(278, 278, -800),
        Point3(278, 278, 0),
        Vec3(0, 1, 0),
        40,
        renderSettings.imageWidth / (double)renderSettings.imageHeight,
        0.05,
        800
    );
    scene->setMainCam(mainCam);

    return scene;
}

int main(int argc, char** argv) {
	argparse::ArgumentParser program("mrRayDemo");

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
	
	RenderSettings renderSettings(width, height, spp, threads, tileSize);
    auto cornell = cornellBox(renderSettings);

    RenderEngine engine;
    engine.init(renderSettings);
    {
        Timer timer("execute");
        engine.execute(renderSettings, cornell.get());
    }
    engine.getFilm()->writeToFile(out);
	return 0;
}