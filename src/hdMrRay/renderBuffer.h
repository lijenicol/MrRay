#ifndef HD_MR_RAY_RENDERBUFFER_H
#define HD_MR_RAY_RENDERBUFFER_H

#include "pxr/pxr.h"
#include <pxr/imaging/hd/renderBuffer.h>

PXR_NAMESPACE_OPEN_SCOPE

class HdMrRayRenderBuffer final : public HdRenderBuffer
{
public:
    HdMrRayRenderBuffer(SdfPath const &id);
    ~HdMrRayRenderBuffer() override;

    void Sync(
        HdSceneDelegate *sceneDelegate, HdRenderParam *renderParam,
        HdDirtyBits *dirtyBits) override;

    void Finalize(HdRenderParam *renderParam) override;

    bool Allocate(
        const GfVec3i &dimensions, HdFormat format, bool multiSampled) override;

    unsigned int GetWidth() const override { return _width; }

    unsigned int GetHeight() const override { return _height; }

    unsigned int GetDepth() const override { return 1; }

    HdFormat GetFormat() const override { return _format; }

    bool IsMultiSampled() const override { return false; }

    void *Map() override;

    void Unmap() override;

    bool IsMapped() const override;

    void Resolve() override;

    bool IsConverged() const override;

    void SetConverged(bool converged) { _converged.store(converged); }

protected:
    void _Deallocate() override;

    unsigned int _width;
    unsigned int _height;
    HdFormat _format;

    std::atomic<int> _mappers;
    std::atomic<bool> _converged;
    std::vector<float> _buffer;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // HD_MR_RAY_RENDERBUFFER_H
