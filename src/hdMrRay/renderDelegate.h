//
// Created by lijenicol on 13/06/23.
//

#ifndef HD_MR_RAY_RENDER_DELEGATE_H
#define HD_MR_RAY_RENDER_DELEGATE_H

#include <pxr/imaging/hd/renderDelegate.h>
#include <pxr/imaging/hd/renderThread.h>

#include <mrRay/renderEngine.h>
#include <mrRay/scene.h>

#include "renderer.h"
#include "renderParam.h"

PXR_NAMESPACE_OPEN_SCOPE

class HdMrRayRenderDelegate final: public HdRenderDelegate
{
public:
    HdMrRayRenderDelegate();
    HdMrRayRenderDelegate(HdRenderSettingsMap const& settingsMap);

    virtual ~HdMrRayRenderDelegate();

    const TfTokenVector &GetSupportedRprimTypes() const override;
    const TfTokenVector &GetSupportedSprimTypes() const override;
    const TfTokenVector &GetSupportedBprimTypes() const override;

    HdResourceRegistrySharedPtr GetResourceRegistry() const override;

    HdRenderPassSharedPtr CreateRenderPass(
        HdRenderIndex *index, const HdRprimCollection &collection) override;

    HdInstancer *CreateInstancer(
        HdSceneDelegate* delegate, SdfPath const& id) override;

    void DestroyInstancer(HdInstancer* instancer) override;
    HdRprim *CreateRprim(
        const TfToken &typeId, const SdfPath &rprimId) override;

    void DestroyRprim(HdRprim *rPrim) override;
    HdSprim *CreateSprim(
        const TfToken &typeId, const SdfPath &sprimId) override;

    HdSprim *CreateFallbackSprim(const TfToken &typeId) override;
    void DestroySprim(HdSprim *sprim) override;
    HdBprim *CreateBprim(
        const TfToken &typeId, const SdfPath &bprimId) override;

    HdBprim *CreateFallbackBprim(const TfToken &typeId) override;
    void DestroyBprim(HdBprim *bprim) override;

    void CommitResources(HdChangeTracker *tracker) override;

    HdRenderParam *GetRenderParam() const override;

    HdAovDescriptor GetDefaultAovDescriptor(const TfToken &name) const override;

private:
    void Initialize();

    static const TfTokenVector SUPPORTED_RPRIM_TYPES;
    static const TfTokenVector SUPPORTED_SPRIM_TYPES;
    static const TfTokenVector SUPPORTED_BPRIM_TYPES;

    HdRenderThread _renderThread;
    HdMrRayRenderer _renderer;
    std::shared_ptr<HdMrRayRenderParam> _renderParam;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif //HD_MR_RAY_RENDER_DELEGATE_H
