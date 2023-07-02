#include <iostream>
#include <thread>

#include "argparse/argparse.hpp"

#include "ray/Camera.h"
#include "ray/rtutils.h"
#include "ray/renderEngine.h"
#include "ray/timer.h"

RAY_NAMESPACE_USING_DIRECTIVE

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
    Scene cornell = cornellBox(defaultCamera(renderSettings));

    RenderEngine engine;
    engine.init(renderSettings);
    {
        Timer timer("execute");
        engine.execute(renderSettings, cornell);
    }
    engine.getFilm()->writeToFile(renderSettings.outputPath);
	return 0;
}