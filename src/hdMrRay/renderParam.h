#ifndef HD_MR_RAY_RENDER_PARAM_H
#define HD_MR_RAY_RENDER_PARAM_H

#include "pxr/imaging/hd/renderDelegate.h"
#include "pxr/imaging/hd/renderThread.h"
#include "pxr/pxr.h"

#include <mrRay/scene.h>

PXR_NAMESPACE_OPEN_SCOPE

class HdMrRayRenderParam final : public HdRenderParam
{
public:
    HdMrRayRenderParam(mrRay::Scene *scene, HdRenderThread *renderThread)
        : _scene(scene)
        , _renderThread(renderThread)
    {
    }

    mrRay::Scene *AcquireSceneForEdit()
    {
        _renderThread->StopRender();
        return _scene;
    }

private:
    mrRay::Scene *_scene;
    HdRenderThread *_renderThread;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // HD_MR_RAY_RENDER_PARAM_H
