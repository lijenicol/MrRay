#ifndef HD_MR_RAY_RENDERER_H
#define HD_MR_RAY_RENDERER_H

#include "pxr/pxr.h"

#include "pxr/imaging/hd/renderPassState.h"
#include "pxr/imaging/hd/renderThread.h"

#include <mrRay/renderEngine.h>

PXR_NAMESPACE_OPEN_SCOPE

class HdMrRayRenderer final
{
public:
    HdMrRayRenderer();
    ~HdMrRayRenderer();

    void SetAovBindings(HdRenderPassAovBindingVector const &aovBindings)
    {
        _aovBindings = aovBindings;
    }

    HdRenderPassAovBindingVector const &GetAovBindings() const
    {
        return _aovBindings;
    }

    void SetDataWindow(const GfRect2i &dataWindow) { _dataWindow = dataWindow; }

    void SetCamera(const HdCamera &camera);

    mrRay::Scene *GetScene() { return &_scene; }

    void SetSamplesPerPixel(unsigned int spp) { _samplesPerPixel = spp; }

    void SetTileSize(unsigned int tileSize) { _tileSize = tileSize; }

    void SetRenderThreads(unsigned int threads) { _renderThreads = threads; }

    void Render(HdRenderThread *renderThread);

private:
    HdRenderPassAovBindingVector _aovBindings;

    GfRect2i _dataWindow;

    mrRay::RenderEngine _engine;
    mrRay::Scene _scene;

    unsigned int _samplesPerPixel;
    unsigned int _tileSize;
    unsigned int _renderThreads;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // HD_MR_RAY_RENDERER_H
