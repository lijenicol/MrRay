//
// Created by lijenicol on 18/06/23.
//

#include <iostream>
#include <pxr/base/gf/vec3i.h>
#include "renderBuffer.h"

PXR_NAMESPACE_USING_DIRECTIVE

HdRayRenderBuffer::HdRayRenderBuffer(SdfPath const& id)
    : HdRenderBuffer(id), _width(0), _height(0), _buffer(),
      _mappers(0), _converged(false)
{
}

HdRayRenderBuffer::~HdRayRenderBuffer() = default;

void
HdRayRenderBuffer::Sync(
    HdSceneDelegate *sceneDelegate, HdRenderParam *renderParam,
    HdDirtyBits *dirtyBits)
{
    HdRenderBuffer::Sync(sceneDelegate, renderParam, dirtyBits);
}

void
HdRayRenderBuffer::Finalize(HdRenderParam *renderParam)
{
    HdRenderBuffer::Finalize(renderParam);
}

bool
HdRayRenderBuffer::Allocate(
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

void
*HdRayRenderBuffer::Map()
{
    _mappers++;
    return _buffer.data();
}

void
HdRayRenderBuffer::Unmap()
{
    _mappers--;
}

bool
HdRayRenderBuffer::IsMapped() const
{
    return _mappers.load() != 0;
}

void
HdRayRenderBuffer::Resolve()
{
}

bool
HdRayRenderBuffer::IsConverged() const
{
    return _converged.load();
}

void
HdRayRenderBuffer::_Deallocate()
{
    _width = 0;
    _height = 0;
    _format = HdFormatInvalid;
    _buffer.resize(0);
    _mappers.store(0);
    _converged.store(false);
}
