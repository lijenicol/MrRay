//
// Created by lijenicol on 26/07/23.
//

#ifndef HDRAY_RENDERPARAM_H
#define HDRAY_RENDERPARAM_H

#include "pxr/pxr.h"
#include "pxr/imaging/hd/renderDelegate.h"
#include "pxr/imaging/hd/renderThread.h"

#include <ray/scene.h>

PXR_NAMESPACE_OPEN_SCOPE

class HdRayRenderParam final : public HdRenderParam
{
public:
    HdRayRenderParam(
        ray::Scene *scene, HdRenderThread *renderThread)
        : _scene(scene), _renderThread(renderThread)
    {}

    ray::Scene* AcquireSceneForEdit() {
        _renderThread->StopRender();
        return _scene;
    }

private:
    ray::Scene *_scene;
    HdRenderThread *_renderThread;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif //HDRAY_RENDERPARAM_H
