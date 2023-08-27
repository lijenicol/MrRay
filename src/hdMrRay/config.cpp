#include <thread>

#include "config.h"

#include "pxr/base/tf/instantiateSingleton.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_INSTANTIATE_SINGLETON(HdMrRayConfig);

HdMrRayConfig::HdMrRayConfig()
    : samplesPerPixel(1u)
    , tileSize(64u)
    , threads(std::thread::hardware_concurrency())
{
}

const HdMrRayConfig &
HdMrRayConfig::GetInstance()
{
    return TfSingleton<HdMrRayConfig>::GetInstance();
}

PXR_NAMESPACE_CLOSE_SCOPE
