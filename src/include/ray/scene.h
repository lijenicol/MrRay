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
    Scene(const Camera& mainCam,
          const std::shared_ptr<Texture>& skyboxTexture) 
        : mainCam(mainCam),
          skyboxTexture(skyboxTexture),
          rawHittables(std::make_shared<HittableList>()) {}
    const Camera mainCam;
    const std::shared_ptr<Texture> skyboxTexture;

    /// Initializes the scene, making it ready for rendering
    void init();
    /// Add the given hittable to the scene
    void addHittable(const std::shared_ptr<Hittable>& hittable);
    /// Add the given hittables to the scene
    void addHittables(const std::shared_ptr<HittableList>& hittableList);
    /// Returns the world to use for ray intersections
    Hittable* getWorld() const;

private:
    std::shared_ptr<Hittable> world;
    std::shared_ptr<HittableList> rawHittables;
};

/// Simple scene with one sphere
Scene simpleSphere(const Camera& mainCam);
/// Cornell box scene
Scene cornellBox(const Camera& mainCam);

RAY_NAMESPACE_CLOSE_SCOPE

#endif
