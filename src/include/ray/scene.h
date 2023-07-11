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
    Scene();

    const std::shared_ptr<Texture> skyboxTexture;

    /// Initializes the scene, making it ready for rendering
    void init();
    /// Add the given hittable to the scene
    void addHittable(const std::shared_ptr<Hittable>& hittable);
    /// Add the given hittables to the scene
    void addHittables(const HittableList& hittableList);
    /// Set the camera to render from
    void setMainCam(const std::shared_ptr<Camera>& camera)
        { _mainCamera = camera; };
    /// Returns the camera to render from
    Camera *getMainCam() const { return _mainCamera.get(); }
    /// Returns the world to use for ray intersections
    Hittable* getWorld() const;

private:
    std::shared_ptr<Camera> _mainCamera;
    std::shared_ptr<Hittable> _world;
    std::shared_ptr<HittableList> _rawHittables;
};

RAY_NAMESPACE_CLOSE_SCOPE

#endif
