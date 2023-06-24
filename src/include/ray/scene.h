#ifndef SCENE_H
#define SCENE_H

#include <memory>

#include "ray/namespace.h"
#include "ray/Camera.h"
#include "ray/geom/Sphere.h"
#include "ray/geom/HittableList.h"
#include "ray/material/Texture.h"

RAY_NAMESPACE_OPEN_SCOPE

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

RAY_NAMESPACE_CLOSE_SCOPE

#endif
