#include "renderDelegate.h"

#include "renderPass.h"
#include "mesh.h"

#include <pxr/imaging/hd/camera.h>

PXR_NAMESPACE_OPEN_SCOPE

const TfTokenVector HdMrRayRenderDelegate::SUPPORTED_RPRIM_TYPES =
{
    HdPrimTypeTokens->mesh,
};

const TfTokenVector HdMrRayRenderDelegate::SUPPORTED_SPRIM_TYPES =
{
    HdPrimTypeTokens->camera,
};

const TfTokenVector HdMrRayRenderDelegate::SUPPORTED_BPRIM_TYPES =
{
    HdPrimTypeTokens->renderBuffer,
};

static void
_RenderCallback(
    HdMrRayRenderer *renderer, HdRenderThread *renderThread)
{
    renderer->Render(renderThread);
}

HdMrRayRenderDelegate::HdMrRayRenderDelegate()
    : HdRenderDelegate()
    , _renderer()
    , _renderThread()
{
    Initialize();
}

HdMrRayRenderDelegate::HdMrRayRenderDelegate(
    const HdRenderSettingsMap &settingsMap)
    : HdRenderDelegate(settingsMap)
{
    Initialize();
}

void
HdMrRayRenderDelegate::Initialize()
{
    _renderParam = std::make_shared<HdMrRayRenderParam>(
        _renderer.GetScene(), &_renderThread);

    _renderThread.SetRenderCallback(
        std::bind(_RenderCallback, &_renderer, &_renderThread));
    _renderThread.StartThread();
}

HdMrRayRenderDelegate::~HdMrRayRenderDelegate()
{
}

const TfTokenVector &
HdMrRayRenderDelegate::GetSupportedRprimTypes() const
{
    return SUPPORTED_RPRIM_TYPES;
}

const TfTokenVector &
HdMrRayRenderDelegate::GetSupportedSprimTypes() const
{
    return SUPPORTED_SPRIM_TYPES;
}

const TfTokenVector &
HdMrRayRenderDelegate::GetSupportedBprimTypes() const
{
    return SUPPORTED_BPRIM_TYPES;
}

HdResourceRegistrySharedPtr
HdMrRayRenderDelegate::GetResourceRegistry() const
{
    return HdResourceRegistrySharedPtr();
}

HdRenderPassSharedPtr
HdMrRayRenderDelegate::CreateRenderPass(
        HdRenderIndex *index, const HdRprimCollection &collection)
{
    return std::make_shared<HdMrRayRenderPass>(
        index, collection, &_renderThread, &_renderer);
}

HdInstancer*
HdMrRayRenderDelegate::CreateInstancer(
    HdSceneDelegate *delegate, const SdfPath &id)
{
    TF_CODING_ERROR("Creating instancer not supported");
    return nullptr;
}

void
HdMrRayRenderDelegate::DestroyInstancer(HdInstancer *instancer)
{
    TF_CODING_ERROR("Destroy instancer not supported");
}

HdRprim*
HdMrRayRenderDelegate::CreateRprim(const TfToken &typeId, const SdfPath &rprimId)
{
    if (typeId == HdPrimTypeTokens->mesh) {
        return new HdMrRayMesh(rprimId);
    }
    return nullptr;
}

void
HdMrRayRenderDelegate::DestroyRprim(HdRprim *rPrim)
{
    delete rPrim;
}

HdSprim*
HdMrRayRenderDelegate::CreateSprim(const TfToken &typeId, const SdfPath &sprimId)
{
    if (typeId == HdPrimTypeTokens->camera) {
        return new HdCamera(sprimId);
    }
    return nullptr;
}

HdSprim*
HdMrRayRenderDelegate::CreateFallbackSprim(const TfToken &typeId)
{
    if (typeId == HdPrimTypeTokens->camera) {
        return new HdCamera(SdfPath::EmptyPath());
    }
    return nullptr;
}

void
HdMrRayRenderDelegate::DestroySprim(HdSprim *sPrim)
{
    delete sPrim;
}

HdBprim*
HdMrRayRenderDelegate::CreateBprim(const TfToken &typeId, const SdfPath &bprimId)
{
    if (typeId == HdPrimTypeTokens->renderBuffer) {
        return new HdMrRayRenderBuffer(bprimId);
    }
    return nullptr;
}

HdBprim*
HdMrRayRenderDelegate::CreateFallbackBprim(const TfToken &typeId)
{
    if (typeId == HdPrimTypeTokens->renderBuffer) {
        return new HdMrRayRenderBuffer(SdfPath::EmptyPath());
    }
    return nullptr;
}

void
HdMrRayRenderDelegate::DestroyBprim(HdBprim *bprim)
{
    delete bprim;
}

HdRenderParam*
HdMrRayRenderDelegate::GetRenderParam() const
{
    return _renderParam.get();
}

void
HdMrRayRenderDelegate::CommitResources(HdChangeTracker *tracker)
{
}

HdAovDescriptor
HdMrRayRenderDelegate::GetDefaultAovDescriptor(const TfToken &name) const
{
    if (name == HdAovTokens->color) {
        return HdAovDescriptor(HdFormatUNorm8Vec4, false,
                               VtValue(GfVec4f(0.f)));
    }
    return HdAovDescriptor();
}

PXR_NAMESPACE_CLOSE_SCOPE