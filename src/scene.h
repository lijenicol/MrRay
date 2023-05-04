#ifndef SCENE_H
#define SCENE_H

#include <memory>

#include "Camera.h"
#include "geom/Sphere.h"
#include "geom/HittableList.h"
#include "material/Texture.h"

class Scene
{
public:
    Scene(const Camera& mainCam, const std::shared_ptr<Hittable>& world,
          const std::shared_ptr<Texture>& skyboxTexture) 
        : mainCam(mainCam), world(world), 
          skyboxTexture(skyboxTexture) {}
    const Camera mainCam; 
    const std::shared_ptr<Hittable> world;
    const std::shared_ptr<Texture> skyboxTexture;
};

Scene simpleSphere(const Camera& mainCam);
Scene cornellBox(const Camera& mainCam);

#endif
