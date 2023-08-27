#ifndef HD_MR_RAY_MESH_H
#define HD_MR_RAY_MESH_H

#include "pxr/base/gf/matrix4f.h"
#include "pxr/imaging/hd/enums.h"
#include "pxr/imaging/hd/mesh.h"
#include "pxr/imaging/hd/vertexAdjacency.h"
#include "pxr/pxr.h"

#include <mrRay/geom/mesh.h>

PXR_NAMESPACE_OPEN_SCOPE

class HdMrRayMesh final : public HdMesh
{
public:
    HdMrRayMesh(SdfPath const &id);

    virtual ~HdMrRayMesh() = default;

    virtual HdDirtyBits GetInitialDirtyBitsMask() const override;

    virtual void Sync(
        HdSceneDelegate *sceneDelegate, HdRenderParam *renderParam,
        HdDirtyBits *dirtyBits, TfToken const &reprToken) override;

    virtual void Finalize(HdRenderParam *renderParam) override;

protected:
    virtual void
    _InitRepr(TfToken const &reprToken, HdDirtyBits *dirtyBits) override;

    virtual HdDirtyBits _PropagateDirtyBits(HdDirtyBits bits) const override;

private:
    HdMeshTopology _topology;
    GfMatrix4d _transform;
    VtVec3fArray _points;

    std::shared_ptr<mrRay::Mesh> _rayMesh;

    HdMrRayMesh(const HdMrRayMesh &) = delete;
    HdMrRayMesh &operator=(const HdMrRayMesh &) = delete;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // HD_MR_RAY_MESH_H
