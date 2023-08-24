//
// Created by lijenicol on 26/07/23.
//

#include "mesh.h"
#include "renderParam.h"
#include <mrRay/material/material.h>
#include <mrRay/matrix.h>

PXR_NAMESPACE_OPEN_SCOPE

HdMrRayMesh::HdMrRayMesh(const SdfPath &id)
    : HdMesh(id)
    , _topology()
    , _transform()
    , _points()
    , _rayMesh()
{
}

void
HdMrRayMesh::Finalize(HdRenderParam *renderParam)
{
}

HdDirtyBits
HdMrRayMesh::GetInitialDirtyBitsMask() const
{
    int mask = HdChangeTracker::Clean | HdChangeTracker::InitRepr
             | HdChangeTracker::DirtyPoints | HdChangeTracker::DirtyTopology
             | HdChangeTracker::DirtyTransform | HdChangeTracker::DirtyVisibility
             | HdChangeTracker::DirtyCullStyle | HdChangeTracker::DirtyDoubleSided
             | HdChangeTracker::DirtyDisplayStyle
             | HdChangeTracker::DirtySubdivTags | HdChangeTracker::DirtyPrimvar
             | HdChangeTracker::DirtyNormals | HdChangeTracker::DirtyInstancer;

    return (HdDirtyBits)mask;
}

HdDirtyBits
HdMrRayMesh::_PropagateDirtyBits(HdDirtyBits bits) const
{
    return bits;
}

void
HdMrRayMesh::_InitRepr(TfToken const &reprToken, HdDirtyBits *dirtyBits)
{
    // Create an empty repr.
    _ReprVector::iterator it
        = std::find_if(_reprs.begin(), _reprs.end(), _ReprComparator(reprToken));
    if (it == _reprs.end()) {
        _reprs.emplace_back(reprToken, HdReprSharedPtr());
    }
}

void
HdMrRayMesh::Sync(
    HdSceneDelegate *sceneDelegate, HdRenderParam *renderParam,
    HdDirtyBits *dirtyBits, TfToken const &reprToken)
{
    auto *mrRayRenderParam = static_cast<HdMrRayRenderParam *>(renderParam);
    mrRay::Scene *scene = mrRayRenderParam->AcquireSceneForEdit();

    SdfPath const &id = GetId();
    if (HdChangeTracker::IsPrimvarDirty(*dirtyBits, id, HdTokens->points)) {
        VtValue value = GetPoints(sceneDelegate);
        _points = value.Get<VtVec3fArray>();
    }
    if (HdChangeTracker::IsTopologyDirty(*dirtyBits, id)) {
        _topology = GetMeshTopology(sceneDelegate);
    }
    if (HdChangeTracker::IsTransformDirty(*dirtyBits, id)) {
        _transform = sceneDelegate->GetTransform(id).GetTranspose();
    }

    std::shared_ptr<mrRay::Lambertian> meshMaterial;
    if (HdChangeTracker::IsPrimvarDirty(*dirtyBits, id, HdTokens->displayColor)) {
        VtValue value = GetPrimvar(sceneDelegate, HdTokens->displayColor);
        for (auto const &color: value.Get<VtVec3fArray>()) {
            meshMaterial = std::make_shared<mrRay::Lambertian>(
                mrRay::Colour(color[0], color[1], color[2]));
            break;
        }
    }
    if (!meshMaterial) {
        meshMaterial
            = std::make_shared<mrRay::Lambertian>(mrRay::Colour(0.9, 0.9, 0.9));
    }

    mrRay::RawMeshInfo rawMeshInfo;
    for (auto const &point: _points) {
        rawMeshInfo.positions.emplace_back(point[0], point[1], point[2]);
    }
    size_t indicesIndex = 0;
    VtIntArray indices = _topology.GetFaceVertexIndices();
    for (auto const &count: _topology.GetFaceVertexCounts()) {
        // Turn polygons into triangles using "fan triangulation"
        // Note: This only works for convex polys
        for (size_t index = 1; index < count - 1; ++index) {
            rawMeshInfo.positionIndices.push_back(indices[indicesIndex]);
            rawMeshInfo.positionIndices.push_back(indices[indicesIndex + index]);
            rawMeshInfo.positionIndices.push_back(
                indices[indicesIndex + index + 1]);
        }
        indicesIndex += count;
    }

    _rayMesh = std::make_shared<mrRay::Mesh>(
        rawMeshInfo,
        mrRay::Mat4(
            _transform[0][0],
            _transform[0][1],
            _transform[0][2],
            _transform[0][3],
            _transform[1][0],
            _transform[1][1],
            _transform[1][2],
            _transform[1][3],
            _transform[2][0],
            _transform[2][1],
            _transform[2][2],
            _transform[2][3],
            _transform[3][0],
            _transform[3][1],
            _transform[3][2],
            _transform[3][3]),
        false,
        meshMaterial);
    scene->addHittables(*_rayMesh->getTriangles());
}

PXR_NAMESPACE_CLOSE_SCOPE