//
// Created by lijenicol on 20/08/23.
//

#ifndef HD_MR_RAY_CONFIG_H
#define HD_MR_RAY_CONFIG_H

#include "pxr/base/tf/singleton.h"
#include "pxr/pxr.h"

PXR_NAMESPACE_OPEN_SCOPE

class HdMrRayConfig
{
public:
    static const HdMrRayConfig &GetInstance();

    /// Amount of pixel samples to take
    unsigned int samplesPerPixel;

    /// Size of render tiles
    unsigned int tileSize;

    /// Amount of threads to use while rendering
    unsigned int threads;

private:
    HdMrRayConfig();
    ~HdMrRayConfig() = default;

    HdMrRayConfig(const HdMrRayConfig &) = delete;
    HdMrRayConfig &operator=(const HdMrRayConfig &) = delete;

    friend class TfSingleton<HdMrRayConfig>;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // HD_MR_RAY_CONFIG_H
