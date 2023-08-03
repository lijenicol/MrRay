//
// Created by lijenicol on 13/06/23.
//

#include "rendererPlugin.h"
#include "renderDelegate.h"

#include <pxr/imaging/hd/rendererPluginRegistry.h>

PXR_NAMESPACE_OPEN_SCOPE

// Register the ray plugin with the renderer plugin system.
TF_REGISTRY_FUNCTION(TfType)
{
    HdRendererPluginRegistry::Define<HdRayRendererPlugin>();
}

HdRenderDelegate*
HdRayRendererPlugin::CreateRenderDelegate()
{
    return new HdRayRenderDelegate();
}

HdRenderDelegate*
HdRayRendererPlugin::CreateRenderDelegate(
    HdRenderSettingsMap const& settingsMap)
{
    return new HdRayRenderDelegate(settingsMap);
}

void
HdRayRendererPlugin::DeleteRenderDelegate(HdRenderDelegate *renderDelegate)
{
    delete renderDelegate;
}

bool
HdRayRendererPlugin::IsSupported(bool /* gpuEnabled */) const
{
    return true;
}

PXR_NAMESPACE_CLOSE_SCOPE