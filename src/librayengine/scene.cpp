#include "ray/scene.h"

#include <memory>

#include "ray/material/Material.h"

#include "ray/geom/BVHNode.h"

RAY_NAMESPACE_OPEN_SCOPE

Scene::Scene()
    : _mainCamera(nullptr),
      _skyboxTexture(std::make_shared<SolidColour>(0, 0, 0)),
      _rawHittables(std::make_shared<HittableList>()),
      _hittableListDirty(false)
{}

void
Scene::init()
{
    if (_hittableListDirty) {
        // On small scales, a BVH will perform worse; however, on
        // the larger scale, it is a lot faster
        _world = std::make_shared<BVHNode>(*_rawHittables, 0, 0);
        _hittableListDirty = false;
    }
}

void
Scene::addHittable(const std::shared_ptr<Hittable>& hittable)
{
    _rawHittables->add(hittable);
    _hittableListDirty = true;
}

void
Scene::addHittables(const HittableList& hittableList)
{
    for (auto const& hittable : hittableList.objects)
    {
        this->addHittable(hittable);
    }
}

Hittable*
Scene::getWorld() const
{
    return _world.get();
}

RAY_NAMESPACE_CLOSE_SCOPE
