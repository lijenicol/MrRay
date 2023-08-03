//
// Created by lijenicol on 26/07/23.
//

#ifndef HDRAY_MESH_H
#define HDRAY_MESH_H

#include "pxr/pxr.h"
#include "pxr/imaging/hd/mesh.h"
#include "pxr/imaging/hd/enums.h"
#include "pxr/imaging/hd/vertexAdjacency.h"
#include "pxr/base/gf/matrix4f.h"

#include <ray/geom/mesh.h>

PXR_NAMESPACE_OPEN_SCOPE

class HdRayMesh final : public HdMesh
{
public:
    HdRayMesh(SdfPath const& id);

    virtual ~HdRayMesh() = default;

    virtual HdDirtyBits GetInitialDirtyBitsMask() const override;

    virtual void Sync(HdSceneDelegate* sceneDelegate,
                      HdRenderParam*   renderParam,
                      HdDirtyBits*     dirtyBits,
                      TfToken const    &reprToken) override;

    virtual void Finalize(HdRenderParam *renderParam) override;

protected:
    virtual void _InitRepr(TfToken const &reprToken,
                           HdDirtyBits *dirtyBits) override;

    virtual HdDirtyBits _PropagateDirtyBits(HdDirtyBits bits) const override;

private:
    HdMeshTopology _topology;
    GfMatrix4d _transform;
    VtVec3fArray _points;

    std::shared_ptr<ray::Mesh> _rayMesh;

    HdRayMesh(const HdRayMesh&)             = delete;
    HdRayMesh &operator =(const HdRayMesh&) = delete;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif //HDRAY_MESH_H
