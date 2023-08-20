//
// Created by lijenicol on 15/06/23.
//

#ifndef HD_MR_RAY_RENDER_PASS_H
#define HD_MR_RAY_RENDER_PASS_H

#include "renderBuffer.h"
#include "renderer.h"

#include <pxr/imaging/hd/renderPass.h>
#include <pxr/imaging/hd/renderThread.h>

PXR_NAMESPACE_OPEN_SCOPE

class HdMrRayRenderPass final : public HdRenderPass
{
public:
    HdMrRayRenderPass(
        HdRenderIndex *index, const HdRprimCollection &collection,
        HdRenderThread* renderThread, HdMrRayRenderer *renderer);
    ~HdMrRayRenderPass() override;

    bool IsConverged() const override;

protected:
    void _Execute(
        const HdRenderPassStateSharedPtr &renderPassState,
        const TfTokenVector &renderTags) override;

private:
    HdRenderThread *_renderThread;
    HdMrRayRenderer *_renderer;

    unsigned int _lastSettingsVersion;

    // Previous data window
    GfRect2i _dataWindow;

    GfMatrix4d _viewMatrix;
    GfMatrix4d _projMatrix;

    HdRenderPassAovBindingVector _aovBindings;
    HdMrRayRenderBuffer _colorBuffer;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif //HD_MR_RAY_RENDER_PASS_H
