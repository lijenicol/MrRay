//
// Created by lijenicol on 12/07/23.
//

#ifndef HDRAY_RENDERER_H
#define HDRAY_RENDERER_H

#include "pxr/pxr.h"

#include "pxr/imaging/hd/renderThread.h"
#include "pxr/imaging/hd/renderPassState.h"

#include <ray/renderEngine.h>

PXR_NAMESPACE_OPEN_SCOPE

class HdRayRenderer final
{
public:
    HdRayRenderer();
    ~HdRayRenderer();

    void SetAovBindings(HdRenderPassAovBindingVector const &aovBindings)
        { _aovBindings = aovBindings; }
    void SetDataWindow(const GfRect2i &dataWindow)
        { _dataWindow = dataWindow; }
    void SetCamera(const HdCamera &camera);
    ray::Scene* GetScene() { return &_scene; }

    void Render(HdRenderThread *renderThread);

private:
    // The bound aovs for this renderer.
    HdRenderPassAovBindingVector _aovBindings;

    GfRect2i _dataWindow;

    ray::RenderEngine _engine;
    ray::Scene _scene;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif //HDRAY_RENDERER_H
