#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <mutex>

#include "mrRay/namespace.h"
#include "mrRay/camera.h"
#include "mrRay/geom/hittableList.h"
#include "mrRay/material/texture.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

class Scene
{
public:
    Scene();

    /// Initializes the scene, making it ready for rendering
    void init();
    /// Add the given hittable to the scene
    void addHittable(const std::shared_ptr<Hittable>& hittable);
    /// Add the given hittables to the scene
    void addHittables(const HittableList& hittableList);
    /// Set the camera to render from
    void setMainCam(const std::shared_ptr<Camera>& camera)
        { _mainCamera = camera; }
    /// Returns the camera to render from
    Camera *getMainCam() const { return _mainCamera.get(); }
    /// Returns the world to use for ray intersections
    Hittable* getWorld() const;
    /// Set the skybox texture to use
    void setSkyboxTexture(const std::shared_ptr<Texture>& skyboxTexture)
        { _skyboxTexture = skyboxTexture; };
    /// Returns the scene's skybox texture
    Texture *getSkyboxTexture() const { return _skyboxTexture.get(); }

private:
    std::shared_ptr<Camera> _mainCamera;
    std::shared_ptr<Hittable> _world;
    std::shared_ptr<HittableList> _rawHittables;
    std::shared_ptr<Texture> _skyboxTexture;
    bool _hittableListDirty;
    std::recursive_mutex _sceneMutex;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
};

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif
