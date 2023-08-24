//
// Created by lijenicol on 13/06/23.
//

#ifndef HD_MR_RAY_RENDER_PLUGIN_H
#define HD_MR_RAY_RENDER_PLUGIN_H

#include "pxr/imaging/hd/rendererPlugin.h"

PXR_NAMESPACE_OPEN_SCOPE

class HdMrRayRendererPlugin final : public HdRendererPlugin
{
public:
    HdMrRayRendererPlugin() = default;
    virtual ~HdMrRayRendererPlugin() = default;

    virtual HdRenderDelegate *CreateRenderDelegate() override;
    virtual HdRenderDelegate *
    CreateRenderDelegate(HdRenderSettingsMap const &settingsMap) override;

    virtual void DeleteRenderDelegate(HdRenderDelegate *renderDelegate) override;

    virtual bool IsSupported(bool gpuEnabled = true) const override;

private:
    HdMrRayRendererPlugin(const HdMrRayRendererPlugin &) = delete;
    HdMrRayRendererPlugin &operator=(const HdMrRayRendererPlugin &) = delete;
};


PXR_NAMESPACE_CLOSE_SCOPE

#endif // HD_MR_RAY_RENDER_PLUGIN_H
