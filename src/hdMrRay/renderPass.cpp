//
// Created by lijenicol on 15/06/23.
//

#include "renderPass.h"

#include <pxr/imaging/hd/renderPassState.h>

PXR_NAMESPACE_USING_DIRECTIVE

HdMrRayRenderPass::HdMrRayRenderPass(
    HdRenderIndex *index, const HdRprimCollection &collection,
    HdRenderThread *renderThread, HdMrRayRenderer *renderer)
    : HdRenderPass(index, collection)
    , _colorBuffer(SdfPath::EmptyPath())
    , _renderThread(renderThread)
    , _renderer(renderer)
    , _viewMatrix(1.0f) // == identity
    , _projMatrix(1.0f) // == identity
{
}

HdMrRayRenderPass::~HdMrRayRenderPass()
{
    _renderThread->StopRender();
}

void
HdMrRayRenderPass::_Execute(
    const HdRenderPassStateSharedPtr &renderPassState,
    const TfTokenVector &renderTags)
{
    bool needStartRender = false;

    const CameraUtilFraming &framing = renderPassState->GetFraming();
    GfRect2i dataWindow;
    if (framing.IsValid()) {
        dataWindow = framing.dataWindow;
    } else {
        // For applications that use the old viewport API instead of
        // the new camera framing API.
        const GfVec4f vp = renderPassState->GetViewport();
        dataWindow = GfRect2i(GfVec2i(0), int(vp[2]), int(vp[3]));
    }
    if (_dataWindow != dataWindow) {
        _dataWindow = dataWindow;
        _renderThread->StopRender();
        _renderer->SetDataWindow(_dataWindow);

        if (!framing.IsValid()) {
            // Support clients that do not use the new framing API
            // and do not use AOVs.
            //
            // Note that we do not support the case of using the
            // new camera framing API without using AOVs.
            //
            const GfVec3i dimensions(_dataWindow.GetWidth(),
                                     _dataWindow.GetHeight(),
                                     1);

            _colorBuffer.Allocate(
                dimensions,
                HdFormatUNorm8Vec4,
                /*multiSampled=*/false);
        }

        needStartRender = true;
    }

    // TODO: Make a better way to determine camera changes
    const GfMatrix4d view = renderPassState->GetWorldToViewMatrix();
    const GfMatrix4d proj = renderPassState->GetProjectionMatrix();
    if (_viewMatrix != view || _projMatrix != proj) {
        _viewMatrix = view;
        _projMatrix = proj;

        _renderThread->StopRender();
        _renderer->SetCamera(*renderPassState->GetCamera());
        needStartRender = true;
    }

    HdRenderPassAovBindingVector aovBindings
        = renderPassState->GetAovBindings();
    if (_aovBindings != aovBindings || _renderer->GetAovBindings().empty())
    {
        _aovBindings = aovBindings;

        _renderThread->StopRender();
        if (aovBindings.empty()) {
            HdRenderPassAovBinding colorAov;
            colorAov.aovName = HdAovTokens->color;
            colorAov.clearValue = VtValue(GfVec4f(0.1f, 0.1f, 0.1f, 1.f));
            colorAov.renderBuffer = &_colorBuffer;
            aovBindings.push_back(colorAov);
        }
        _renderer->SetAovBindings(aovBindings);
        needStartRender = true;
    }

    if (needStartRender) {
        _renderThread->StartRender();
    }
}

bool
HdMrRayRenderPass::IsConverged() const
{
    for (size_t i = 0; i < _aovBindings.size(); ++i) {
        if (_aovBindings[i].renderBuffer &&
            !_aovBindings[i].renderBuffer->IsConverged()) {
            return false;
        }
    }
    return true;
}
