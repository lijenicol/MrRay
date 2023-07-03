/**
* A mesh is a collection of triangles. This class loads in a mesh from a .obj file and sets
* up all the triangle instances needed such that the mesh can be rendered. 
 **/
#ifndef RAY_MESH_H
#define RAY_MESH_H

#include "ray/namespace.h"
#include "ray/rtutils.h"
#include "ray/geom/HittableList.h"
#include "ray/geom/Triangle.h"

RAY_NAMESPACE_OPEN_SCOPE

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
        const RawMeshInfo& meshInfo, const Vec3& translation,
        const double& scale, const bool& smooth_shading,
        std::shared_ptr<Material> mat);
    ~Mesh();

    HittableList* getTriangles();

	std::shared_ptr<Material> mat;

private:
    bool smooth_shading;
    Vec3* positions;
    Vec3* normals;
    // FIXME: UVs should probably be Vec2 so we aren't wasting memory
    Vec3* uvs;
    int* positionIndices;
    int* normalIndices;
    int* uvIndices;
    size_t faceCount;
    std::unique_ptr<HittableList> _triangles;
    bool normalsDefined, uvsDefined;
};

RAY_NAMESPACE_CLOSE_SCOPE

#endif