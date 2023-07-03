#include <fstream>
#include <sstream>

#include "ray/geom/mesh.h"

RAY_NAMESPACE_OPEN_SCOPE

Mesh::Mesh(
    const RawMeshInfo& meshInfo, const Vec3& translation, const double& scale,
    const bool& smooth_shading, std::shared_ptr<Material> mat)
    : smooth_shading(smooth_shading), mat(mat),
      _triangles(std::make_unique<HittableList>())
{
    size_t positionCount = meshInfo.positions.size();
    positions = new Vec3[positionCount];
    for (int i = 0; i < positionCount; i++) {
        positions[i] = meshInfo.positions[i] * scale + translation;
    }

    size_t normalCount = meshInfo.normals.size();
    normals = new Vec3[normalCount];
    std::copy(meshInfo.normals.begin(), meshInfo.normals.end(), normals);

    size_t uvCount = meshInfo.uvs.size();
    uvs = new Vec3[uvCount];
    std::copy(meshInfo.uvs.begin(), meshInfo.uvs.end(), uvs);

    size_t positionIndexCount = meshInfo.positionIndices.size();
    positionIndices = new int[positionIndexCount];
    std::copy(meshInfo.positionIndices.begin(), meshInfo.positionIndices.end(),
              positionIndices);

    size_t normalIndexCount = meshInfo.normalIndices.size();
    normalIndices = new int[normalIndexCount];
    std::copy(meshInfo.normalIndices.begin(), meshInfo.normalIndices.end(),
              normalIndices);

    size_t uvIndexCount = meshInfo.uvIndices.size();
    uvIndices = new int[uvIndexCount];
    std::copy(meshInfo.uvIndices.begin(), meshInfo.uvIndices.end(),
              uvIndices);

    faceCount = positionIndexCount / 3;
    normalsDefined = !meshInfo.normals.empty();
    uvsDefined = !meshInfo.uvs.empty();
}

Mesh::~Mesh()
{
    delete[] positions;
    delete[] normals;
    delete[] uvs;
    delete[] positionIndices;
    delete[] normalIndices;
    delete[] uvIndices;
}

HittableList*
Mesh::getTriangles()
{
    if (!_triangles->objects.empty()) {
        return _triangles.get();
    }
    for (size_t i = 0; i < faceCount; i++) {
        auto v0 = std::make_shared<vertex_triangle>();
        auto v1 = std::make_shared<vertex_triangle>();
        auto v2 = std::make_shared<vertex_triangle>();

        v0->pos = positions[positionIndices[i*3]];
        v1->pos = positions[positionIndices[i*3+1]];
        v2->pos = positions[positionIndices[i*3+2]];

        if (normalsDefined) {
            v0->normal = normals[normalIndices[i*3]];
            v1->normal = normals[normalIndices[i*3+1]];
            v2->normal = normals[normalIndices[i*3+2]];
        }

        if (uvsDefined) {
            v0->u = uvs[uvIndices[i*3]].x();
            v0->v = uvs[uvIndices[i*3]].y();
            v1->u = uvs[uvIndices[i*3+1]].x();
            v1->v = uvs[uvIndices[i*3+1]].y();
            v2->u = uvs[uvIndices[i*3+2]].x();
            v2->v = uvs[uvIndices[i*3+2]].y();
        }

        // Construct a triangle. If normals aren't specified, never use
        // smooth shading (this is a bit of a hack)
        std::shared_ptr<Triangle> tri = std::make_shared<Triangle>(
                v0, v1, v2, mat, smooth_shading);
        _triangles->add(tri);
    }
    return _triangles.get();
}

RAY_NAMESPACE_CLOSE_SCOPE
