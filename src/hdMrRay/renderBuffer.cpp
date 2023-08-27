#include "renderBuffer.h"
#include <iostream>
#include <pxr/base/gf/vec3i.h>

PXR_NAMESPACE_USING_DIRECTIVE

HdMrRayRenderBuffer::HdMrRayRenderBuffer(SdfPath const &id)
    : HdRenderBuffer(id)
    , _width(0)
    , _height(0)
    , _buffer()
    , _mappers(0)
    , _converged(false)
{
}

HdMrRayRenderBuffer::~HdMrRayRenderBuffer() = default;

void
HdMrRayRenderBuffer::Sync(
    HdSceneDelegate *sceneDelegate, HdRenderParam *renderParam,
    HdDirtyBits *dirtyBits)
{
    HdRenderBuffer::Sync(sceneDelegate, renderParam, dirtyBits);
}

void
HdMrRayRenderBuffer::Finalize(HdRenderParam *renderParam)
{
    HdRenderBuffer::Finalize(renderParam);
}

bool
HdMrRayRenderBuffer::Allocate(
    const GfVec3i &dimensions, HdFormat format, bool multiSampled)
{
    _Deallocate();
    if (dimensions[2] != 1) {
        std::cout << "Depth must be 1!" << std::endl;
        return false;
    }

    _width = dimensions[0];
    _height = dimensions[1];
    _format = format;

    _buffer.resize(dimensions[0] * dimensions[1] * 4);

    return true;
}

void *
HdMrRayRenderBuffer::Map()
{
    _mappers++;
    return _buffer.data();
}

void
HdMrRayRenderBuffer::Unmap()
{
    _mappers--;
}

bool
HdMrRayRenderBuffer::IsMapped() const
{
    return _mappers.load() != 0;
}

void
HdMrRayRenderBuffer::Resolve()
{
}

bool
HdMrRayRenderBuffer::IsConverged() const
{
    return _converged.load();
}

void
HdMrRayRenderBuffer::_Deallocate()
{
    _width = 0;
    _height = 0;
    _format = HdFormatInvalid;
    _buffer.resize(0);
    _mappers.store(0);
    _converged.store(false);
}
