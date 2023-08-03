//
// Created by lijenicol on 15/06/23.
//

#ifndef HDRAY_RENDERPASS_H
#define HDRAY_RENDERPASS_H

#include "renderBuffer.h"
#include "renderer.h"

#include <pxr/imaging/hd/renderPass.h>
#include <pxr/imaging/hd/renderThread.h>

PXR_NAMESPACE_OPEN_SCOPE

class HdRayRenderPass final : public HdRenderPass
{
public:
    HdRayRenderPass(
        HdRenderIndex *index, const HdRprimCollection &collection,
        HdRenderThread* renderThread, HdRayRenderer *renderer);
    ~HdRayRenderPass() override;

    bool IsConverged() const override;

protected:
    void _Execute(
        const HdRenderPassStateSharedPtr &renderPassState,
        const TfTokenVector &renderTags) override;

private:
    HdRenderThread *_renderThread;
    HdRayRenderer *_renderer;

    // Previous data window
    GfRect2i _dataWindow;

    GfMatrix4d _viewMatrix;
    GfMatrix4d _projMatrix;

    HdRenderPassAovBindingVector _aovBindings;
    HdRayRenderBuffer _colorBuffer;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif //HDRAY_RENDERPASS_H
