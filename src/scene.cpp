#include "scene.h"

#include <memory>

#include "rtutils.h"
#include "Camera.h"

#include "material/Material.h"

#include "geom/Sphere.h"
#include "geom/AARect.h"

Scene simpleSphere(const Camera& mainCam)
{
	std::shared_ptr<SolidColour> skyboxTexture = std::make_shared<SolidColour>(0.1, 0.1, 0.1);

	std::shared_ptr<Lambertian> red = std::make_shared<Lambertian>(Colour(.8, .05, .05));
	std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(Vec3(278, 278, 278), 150, red);

    return Scene(mainCam, sphere, skyboxTexture);
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
	objects->add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));			
	objects->add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));		
	objects->add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));		
	objects->add(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));		
	objects->add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));		
	objects->add(std::make_shared<XZRect>(103, 453, 117, 442, 554, light));	
	objects->add(std::make_shared<Sphere>(Vec3(278, 278, 278), 150, white));	

	return Scene(mainCam, objects, skyboxTexture);
}