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
    std::cout << "Creating render buffer" << std::endl;
}

HdRayRenderBuffer::~HdRayRenderBuffer() = default;

void
HdRayRenderBuffer::Sync(
    HdSceneDelegate *sceneDelegate, HdRenderParam *renderParam,
    HdDirtyBits *dirtyBits)
{
    std::cout << "Syncing render buffer" << std::endl;
    HdRenderBuffer::Sync(sceneDelegate, renderParam, dirtyBits);
}

void
HdRayRenderBuffer::Finalize(HdRenderParam *renderParam)
{
    std::cout << "Finalizing render buffer" << std::endl;
    HdRenderBuffer::Finalize(renderParam);
}

// TODO: Make sure that this allocation is happening
bool
HdRayRenderBuffer::Allocate(
    const GfVec3i &dimensions, HdFormat format, bool multiSampled)
{
    std::cout << "Allocating render buffer" << std::endl;
    _Deallocate();
    if (dimensions[2] != 1) {
        std::cout << "Depth must be 1!" << std::endl;
        return false;
    }

    _width = dimensions[0];
    _height = dimensions[1];
    _format = format;

//    size_t bufferSize = dimensions[0] * dimensions[1] * HdDataSizeOfFormat(format);
    _buffer.resize(dimensions[0] * dimensions[1] * 4);

    return true;
}

void
*HdRayRenderBuffer::Map()
{
    std::cout << "Mapping" << std::endl;
    _mappers++;
    return _buffer.data();
}

void
HdRayRenderBuffer::Unmap()
{
    std::cout << "Unmapping" << std::endl;
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
    std::cout << "RESOLVING" << std::endl;
}

bool
HdRayRenderBuffer::IsConverged() const
{
    std::cout << "Checking Convergence ~~~" << std::endl;
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
