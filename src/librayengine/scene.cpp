#include "ray/scene.h"

#include <memory>

#include "ray/rtutils.h"
#include "ray/Camera.h"

#include "ray/material/Material.h"

#include "ray/geom/Sphere.h"
#include "ray/geom/AARect.h"
#include "ray/geom/BVHNode.h"

RAY_NAMESPACE_OPEN_SCOPE

void
Scene::init()
{
    // On small scales, a BVH will perform worse; however, on
    // the larger scale, it is a lot faster
    world = std::make_shared<BVHNode>(*rawHittables, 0, 0);
//    world = rawHittables;
}

void
Scene::addHittable(const std::shared_ptr<Hittable>& hittable)
{
    rawHittables->add(hittable);
}

Hittable*
Scene::getWorld() const
{
    return world.get();
}

Scene simpleSphere(const Camera& mainCam)
{
	std::shared_ptr<SolidColour> skyboxTexture = std::make_shared<SolidColour>(0.1, 0.1, 0.1);

	std::shared_ptr<Lambertian> red = std::make_shared<Lambertian>(Colour(.8, .05, .05));
	std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(Vec3(278, 278, 278), 150, red);

    Scene scene(mainCam, skyboxTexture);
    scene.addHittable(sphere);

    return scene;
}

Scene cornellBox(const Camera& mainCam) 
{
    std::shared_ptr<SolidColour> skyboxTexture = std::make_shared<SolidColour>(0.0, 0.0, 0.0);

	std::shared_ptr<Lambertian> red = std::make_shared<Lambertian>(Colour(.65, .05, .05));
	std::shared_ptr<Lambertian> white = std::make_shared<Lambertian>(Colour(.73, .73, .73));
	std::shared_ptr<Lambertian> green = std::make_shared<Lambertian>(Colour(.12, .45, .15));
	std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(Colour(12.0, 12.0, 12.0));
	std::shared_ptr<Metal> metal = std::make_shared<Metal>(Colour(0.9, 0.6, 0.1), 0.92);

	std::shared_ptr<HittableList> objects = std::make_shared<HittableList>();
    Scene scene(mainCam, skyboxTexture);
	scene.addHittable(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));
	scene.addHittable(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));
	scene.addHittable(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));
	scene.addHittable(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));
	scene.addHittable(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));
	scene.addHittable(std::make_shared<XZRect>(103, 453, 117, 442, 554, light));
	scene.addHittable(std::make_shared<Sphere>(Vec3(278, 278, 278), 150, white));

	return scene;
}

RAY_NAMESPACE_CLOSE_SCOPE
