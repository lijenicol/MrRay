//
// Created by lijenicol on 15/06/23.
//

#include "renderPass.h"

#include <iostream>
#include <pxr/imaging/hd/renderPassState.h>

PXR_NAMESPACE_USING_DIRECTIVE

HdRayRenderPass::HdRayRenderPass(
    HdRenderIndex *index, const HdRprimCollection &collection,
    HdRenderThread *renderThread, HdRayRenderer *renderer)
    : HdRenderPass(index, collection)
    , _colorBuffer(SdfPath::EmptyPath())
    , _renderThread(renderThread)
    , _renderer(renderer)
    , _viewMatrix(1.0f) // == identity
    , _projMatrix(1.0f) // == identity
{
}

HdRayRenderPass::~HdRayRenderPass()
{
    _renderThread->StopRender();
}

void
HdRayRenderPass::_Execute(
    const HdRenderPassStateSharedPtr &renderPassState,
    const TfTokenVector &renderTags)
{
    std::cout << "Executing render pass" << std::endl;

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
            std::cout << "Creating color AOV" << std::endl;
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
HdRayRenderPass::IsConverged() const
{
    std::cout << "Chhecking convergence" << std::endl;
    for (size_t i = 0; i < _aovBindings.size(); ++i) {
        if (_aovBindings[i].renderBuffer &&
            !_aovBindings[i].renderBuffer->IsConverged()) {
            return false;
        }
    }
    return true;
}
