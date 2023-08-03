#include "renderDelegate.h"

#include <iostream>
#include "renderPass.h"
#include "mesh.h"

#include <pxr/imaging/hd/camera.h>

PXR_NAMESPACE_OPEN_SCOPE

const TfTokenVector HdRayRenderDelegate::SUPPORTED_RPRIM_TYPES =
{
    HdPrimTypeTokens->mesh,
};

const TfTokenVector HdRayRenderDelegate::SUPPORTED_SPRIM_TYPES =
{
    HdPrimTypeTokens->camera,
};

const TfTokenVector HdRayRenderDelegate::SUPPORTED_BPRIM_TYPES =
{
    HdPrimTypeTokens->renderBuffer,
};

static void
_RenderCallback(
    HdRayRenderer *renderer, HdRenderThread *renderThread)
{
    std::cout << "Rendering!" << std::endl;
    renderer->Render(renderThread);
}

HdRayRenderDelegate::HdRayRenderDelegate()
    : HdRenderDelegate()
    , _renderer()
    , _renderThread()
{
    Initialize();
}

HdRayRenderDelegate::HdRayRenderDelegate(
    const HdRenderSettingsMap &settingsMap)
    : HdRenderDelegate(settingsMap)
{
    Initialize();
}

void
HdRayRenderDelegate::Initialize()
{
    std::cout << "Initializing render delegate..." << std::endl;

    _renderParam = std::make_shared<HdRayRenderParam>(
        _renderer.GetScene(), &_renderThread);

    _renderThread.SetRenderCallback(
        std::bind(_RenderCallback, &_renderer, &_renderThread));
    _renderThread.StartThread();
}

HdRayRenderDelegate::~HdRayRenderDelegate()
{
    std::cout << "Destroying render delegate..." << std::endl;
}

const TfTokenVector &
HdRayRenderDelegate::GetSupportedRprimTypes() const
{
    return SUPPORTED_RPRIM_TYPES;
}

const TfTokenVector &
HdRayRenderDelegate::GetSupportedSprimTypes() const
{
    return SUPPORTED_SPRIM_TYPES;
}

const TfTokenVector &
HdRayRenderDelegate::GetSupportedBprimTypes() const
{
    return SUPPORTED_BPRIM_TYPES;
}

HdResourceRegistrySharedPtr
HdRayRenderDelegate::GetResourceRegistry() const
{
    return HdResourceRegistrySharedPtr();
}

HdRenderPassSharedPtr
HdRayRenderDelegate::CreateRenderPass(
        HdRenderIndex *index, const HdRprimCollection &collection)
{
    std::cout << "Creating render pass" << std::endl;
    return std::make_shared<HdRayRenderPass>(
        index, collection, &_renderThread, &_renderer);
}

HdInstancer*
HdRayRenderDelegate::CreateInstancer(
    HdSceneDelegate *delegate, const SdfPath &id)
{
    TF_CODING_ERROR("Creating instancer not supported");
    return nullptr;
}

void
HdRayRenderDelegate::DestroyInstancer(HdInstancer *instancer)
{
    TF_CODING_ERROR("Destroy instancer not supported");
}

HdRprim*
HdRayRenderDelegate::CreateRprim(const TfToken &typeId, const SdfPath &rprimId)
{
    std::cout << "Creating Rprim: " << rprimId << std::endl;
    if (typeId == HdPrimTypeTokens->mesh) {
        return new HdRayMesh(rprimId);
    }
    return nullptr;
}

void
HdRayRenderDelegate::DestroyRprim(HdRprim *rPrim)
{
    delete rPrim;
}

HdSprim*
HdRayRenderDelegate::CreateSprim(const TfToken &typeId, const SdfPath &sprimId)
{
    std::cout << "Creating Sprim: " << sprimId << std::endl;
    if (typeId == HdPrimTypeTokens->camera) {
        return new HdCamera(sprimId);
    }
    return nullptr;
}

HdSprim*
HdRayRenderDelegate::CreateFallbackSprim(const TfToken &typeId)
{
    std::cout << "Creating fallback Sprim: "<< typeId << std::endl;
    if (typeId == HdPrimTypeTokens->camera) {
        return new HdCamera(SdfPath::EmptyPath());
    }
    return nullptr;
}

void
HdRayRenderDelegate::DestroySprim(HdSprim *sPrim)
{
    delete sPrim;
}

HdBprim*
HdRayRenderDelegate::CreateBprim(const TfToken &typeId, const SdfPath &bprimId)
{
    std::cout << "Creating Bprim: " << bprimId << std::endl;
    if (typeId == HdPrimTypeTokens->renderBuffer) {
        return new HdRayRenderBuffer(bprimId);
    }
    return nullptr;
}

HdBprim*
HdRayRenderDelegate::CreateFallbackBprim(const TfToken &typeId)
{
    std::cout << "Creating fallback Bprim: " << typeId << std::endl;
    if (typeId == HdPrimTypeTokens->renderBuffer) {
        return new HdRayRenderBuffer(SdfPath::EmptyPath());
    }
    return nullptr;
}

void
HdRayRenderDelegate::DestroyBprim(HdBprim *bprim)
{
    delete bprim;
}

HdRenderParam*
HdRayRenderDelegate::GetRenderParam() const
{
    std::cout << "Getting render param" << std::endl;
    return _renderParam.get();
}

void
HdRayRenderDelegate::CommitResources(HdChangeTracker *tracker)
{
    std::cout << "Call to CommitResources" << std::endl;
}

HdAovDescriptor
HdRayRenderDelegate::GetDefaultAovDescriptor(const TfToken &name) const
{
    if (name == HdAovTokens->color) {
        std::cout << "Getting aov desc for color" << std::endl;
        return HdAovDescriptor(HdFormatUNorm8Vec4, false,
                               VtValue(GfVec4f(0.f)));
    }
    return HdAovDescriptor();
}

PXR_NAMESPACE_CLOSE_SCOPE