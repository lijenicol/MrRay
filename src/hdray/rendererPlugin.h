//
// Created by lijenicol on 13/06/23.
//

#ifndef HDRAYRENDERERPLUGIN_H
#define HDRAYRENDERERPLUGIN_H

#include "pxr/imaging/hd/rendererPlugin.h"

PXR_NAMESPACE_OPEN_SCOPE

class HdRayRendererPlugin final : public HdRendererPlugin
{
public:
    HdRayRendererPlugin() = default;
    virtual ~HdRayRendererPlugin() = default;

    virtual HdRenderDelegate *CreateRenderDelegate() override;
    virtual HdRenderDelegate *CreateRenderDelegate(
        HdRenderSettingsMap const& settingsMap) override;

    virtual void DeleteRenderDelegate(
        HdRenderDelegate *renderDelegate) override;

    virtual bool IsSupported(bool gpuEnabled = true) const override;
private:
    HdRayRendererPlugin(const HdRayRendererPlugin&) = delete;
    HdRayRendererPlugin &operator =(const HdRayRendererPlugin&) = delete;
};


PXR_NAMESPACE_CLOSE_SCOPE

#endif //HDRAYRENDERERPLUGIN_H
