//
// Created by lijenicol on 12/07/23.
//

#include "renderer.h"
#include "renderBuffer.h"

#include <pxr/imaging/hd/camera.h>

#include <mrRay/material/material.h>

uint8_t remapColor(double colorComponent);

PXR_NAMESPACE_OPEN_SCOPE

HdMrRayRenderer::HdMrRayRenderer()
    : _aovBindings()
    , _engine()
    , _scene()
    , _dataWindow()
{
}

HdMrRayRenderer::~HdMrRayRenderer()
{
}

void
HdMrRayRenderer::SetCamera(const HdCamera &camera)
{
    GfMatrix4d transform = camera.GetTransform();
    GfVec3d translation = transform.ExtractTranslation();
    GfVec4d lookAt = GfVec4d(0, 0, -1, 1) * transform;
    GfVec4d vup = GfVec4d(0, 1, 0, 0) * transform;
    double vfovRadians
        = 2 * atan(
            camera.GetVerticalAperture() / (2 * camera.GetFocalLength()));
    _scene.setMainCam(
        std::make_shared<mrRay::Camera>(
            mrRay::Point3(translation[0], translation[1], translation[2]),
            mrRay::Point3(lookAt[0], lookAt[1], lookAt[2]),
            mrRay::Vec3(vup[0], vup[1], vup[2]),
            vfovRadians * 180 / mrRay::pi,
            _dataWindow.GetWidth() / (double)_dataWindow.GetHeight(),
            // TODO: Correct aperture
            0.0,
            10)
    );
}

void
HdMrRayRenderer::Render(HdRenderThread *renderThread)
{
    // TODO: Should make sure that the buffers don't differ in dimensions.
    //   For now, this should be good as we only have one AOV
    unsigned int width, height;
    GfVec4f clearValue;
    for (auto const& aov : _aovBindings) {
        HdMrRayRenderBuffer *rb
            = static_cast<HdMrRayRenderBuffer*>(aov.renderBuffer);
        width = rb->GetWidth();
        height = rb->GetHeight();
        clearValue =
            *(static_cast<const GfVec4f*>(HdGetValueData(aov.clearValue)));
        rb->SetConverged(false);
    }

    _scene.setSkyboxTexture(
        std::make_shared<mrRay::SolidColour>(
            clearValue[0], clearValue[1], clearValue[2]));

    mrRay::RenderSettings renderSettings(width, height, 10, 20, 64);
    _engine.init(renderSettings);
    _engine.execute(renderSettings, &_scene);

    mrRay::Colour* colours = _engine.getFilm()->getData();
    for (auto const& aov : _aovBindings) {
        HdMrRayRenderBuffer *rb
            = static_cast<HdMrRayRenderBuffer*>(aov.renderBuffer);
        auto *target = (uint8_t*)rb->Map();
        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                size_t index = y * width + x;
                mrRay::Colour colour = colours[index];
                target[index * 4] = remapColor(colour[0]);
                target[index * 4 + 1] = remapColor(colour[1]);
                target[index * 4 + 2] = remapColor(colour[2]);
                target[index * 4 + 3] = 255;
            }
        }
        rb->Unmap();
        rb->SetConverged(true);
    }
}

PXR_NAMESPACE_CLOSE_SCOPE

uint8_t
remapColor(double colorComponent)
{
    return (uint8_t)(255 * mrRay::clamp(colorComponent, 0, 1));
}
