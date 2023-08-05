#include <fstream>
#include <sstream>

#include "mrRay/geom/mesh.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

Mesh::Mesh(
    const RawMeshInfo& meshInfo, const Mat4& objToWorld,
    const bool& smoothShading, std::shared_ptr<Material> mat)
    : smoothShading(smoothShading), mat(mat),
      _triangles(std::make_unique<HittableList>())
{
    size_t positionCount = meshInfo.positions.size();
    positions = new Vec3[positionCount];
    for (int i = 0; i < positionCount; i++) {
        Vec3 position = meshInfo.positions[i];
        // TODO: Matrix multiplication should be implemented elsewhere
        positions[i] = Vec3(
            (objToWorld[0] * position[0]) + (objToWorld[1] * position[1]) + (objToWorld[2] * position[2]) + objToWorld[3],
            (objToWorld[4] * position[0]) + (objToWorld[5] * position[1]) + (objToWorld[6] * position[2]) + objToWorld[7],
            (objToWorld[8] * position[0]) + (objToWorld[9] * position[1]) + (objToWorld[10] * position[2]) + objToWorld[11]
        );
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
        size_t offset = i * 3;
        int* normalIndex = normalsDefined ? normalIndices + offset : nullptr;
        int* uvIndex = uvsDefined ? uvIndices + offset : nullptr;
        std::shared_ptr<Triangle> tri = std::make_shared<Triangle>(
            positionIndices + offset, normalIndex, uvIndex, this, mat);
        _triangles->add(tri);
    }
    return _triangles.get();
}

MR_RAY_NAMESPACE_CLOSE_SCOPE
