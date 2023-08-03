#include "renderDelegate.h"

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
    _renderParam = std::make_shared<HdRayRenderParam>(
        _renderer.GetScene(), &_renderThread);

    _renderThread.SetRenderCallback(
        std::bind(_RenderCallback, &_renderer, &_renderThread));
    _renderThread.StartThread();
}

HdRayRenderDelegate::~HdRayRenderDelegate()
{
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
    if (typeId == HdPrimTypeTokens->camera) {
        return new HdCamera(sprimId);
    }
    return nullptr;
}

HdSprim*
HdRayRenderDelegate::CreateFallbackSprim(const TfToken &typeId)
{
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
    if (typeId == HdPrimTypeTokens->renderBuffer) {
        return new HdRayRenderBuffer(bprimId);
    }
    return nullptr;
}

HdBprim*
HdRayRenderDelegate::CreateFallbackBprim(const TfToken &typeId)
{
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
    return _renderParam.get();
}

void
HdRayRenderDelegate::CommitResources(HdChangeTracker *tracker)
{
}

HdAovDescriptor
HdRayRenderDelegate::GetDefaultAovDescriptor(const TfToken &name) const
{
    if (name == HdAovTokens->color) {
        return HdAovDescriptor(HdFormatUNorm8Vec4, false,
                               VtValue(GfVec4f(0.f)));
    }
    return HdAovDescriptor();
}

PXR_NAMESPACE_CLOSE_SCOPE