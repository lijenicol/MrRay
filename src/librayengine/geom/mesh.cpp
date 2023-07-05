#include <fstream>
#include <sstream>

#include "ray/geom/mesh.h"

RAY_NAMESPACE_OPEN_SCOPE

Mesh::Mesh(
    const RawMeshInfo& meshInfo, const Vec3& translation, const double& scale,
    const bool& smoothShading, std::shared_ptr<Material> mat)
    : smoothShading(smoothShading), mat(mat),
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
        size_t offset = i * 3;
        int* normalIndex = normalsDefined ? normalIndices + offset : nullptr;
        int* uvIndex = uvsDefined ? uvIndices + offset : nullptr;
        std::shared_ptr<Triangle> tri = std::make_shared<Triangle>(
            positionIndices + offset, normalIndex, uvIndex, this, mat);
        _triangles->add(tri);
    }
    return _triangles.get();
}

RAY_NAMESPACE_CLOSE_SCOPE
