/**
 * A mesh is a collection of triangles. This class loads in a mesh from a .obj
 *file and sets up all the triangle instances needed such that the mesh can be
 *rendered.
 **/
#ifndef MR_RAY_MESH_H
#define MR_RAY_MESH_H

#include "mrRay/geom/hittableList.h"
#include "mrRay/geom/triangle.h"
#include "mrRay/matrix.h"
#include "mrRay/namespace.h"
#include "mrRay/rtutils.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

struct RawMeshInfo
{
    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    // FIXME: UVs should probably be Vec2 so we aren't wasting memory
    std::vector<Vec3> uvs;
    std::vector<int> positionIndices;
    std::vector<int> normalIndices;
    std::vector<int> uvIndices;
};

class Mesh
{
public:
    Mesh(
        const RawMeshInfo &meshInfo, const Mat4 &objToWorld,
        const bool &smoothShading, std::shared_ptr<Material> mat);
    ~Mesh();

    HittableList *getTriangles();

    std::shared_ptr<Material> mat;
    Vec3 *positions;
    Vec3 *normals;
    // FIXME: UVs should probably be Vec2 so we aren't wasting memory
    Vec3 *uvs;
    bool smoothShading;

private:
    int *positionIndices;
    int *normalIndices;
    int *uvIndices;
    size_t faceCount;
    std::unique_ptr<HittableList> _triangles;
    bool normalsDefined, uvsDefined;
};

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif // MR_RAY_MESH_H
