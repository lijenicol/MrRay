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

    const GfRect2i dataWindow = renderPassState->GetFraming().dataWindow;
    if (_dataWindow != dataWindow) {
        _dataWindow = dataWindow;
        _renderer->SetDataWindow(_dataWindow);
        _renderThread->StopRender();
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
    if (_aovBindings != aovBindings)
    {
        _aovBindings = aovBindings;

        if (aovBindings.empty()) {
            HdRenderPassAovBinding colorAov;
            colorAov.aovName = HdAovTokens->color;
            colorAov.clearValue = VtValue(GfVec4f(0.1f, 0.f, 0.f, 1.f));
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
