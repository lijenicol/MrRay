//
// Created by lijenicol on 25/06/23.
//

#include <fstream>

#include "ray/geom/meshLoader.h"
#include "ray/material/Material.h"

RAY_NAMESPACE_OPEN_SCOPE

MeshLoader::MeshLoader(const std::string& meshPath)
    : _meshPath(meshPath),
      _hasLoaded(false),
      _triangles(std::make_unique<HittableList>())
{
}

HittableList*
MeshLoader::getTriangles()
{
    if (_hasLoaded) {
        return _triangles.get();
    }
    return nullptr;
}

/// Split a string into multiple pieces at delimiter points
std::vector<std::string> splitString(std::string s, char delimiter)
{
    std::vector<std::string> splits;

    // Find occurrence of white space and split
    size_t pos;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        // Push empty string if delim occurs right away
        if (pos == 0) {
            splits.push_back("");
        }

        std::string token = s.substr(0, pos);
        s.erase(0, pos + 1);
        splits.push_back(token);
    }

    // Add the last part of the string
    splits.push_back(s);
    return splits;
}

OBJLoader::OBJLoader(const std::string &meshPath)
    : MeshLoader(meshPath)
{
}

bool
OBJLoader::loadTriangles()
{
    std::ifstream objStream(_meshPath);
    if(!objStream.is_open()) {
        std::cerr << "File cannot be read or does not exist: " << _meshPath
                  << std::endl;
        return false;
    }

    std::cout << "Creating a default material for mesh" << std::endl;
    std::shared_ptr<Lambertian> defaultMaterial =
            std::make_shared<Lambertian>(Colour(1., 1., 1.));

    std::vector<Vec3> triangleIndices;
    std::vector<Vec3> texCoordIndices;
    std::vector<Vec3> normalIndices;

    std::vector<Vec3> inputPoints;
    std::vector<Vec3> inputTexCoords;
    std::vector<Vec3> inputNormals;

    std::string line;

    while (std::getline(objStream, line)) {
        // Split string
        auto parts = splitString(line, ' ');

        // Parse vertices
        if (parts[0] == "v") {
            //construct a point from the line
            Vec3 point;
            point.e[0] = std::stod(parts[1]);
            point.e[1] = std::stod(parts[2]);
            point.e[2] = std::stod(parts[3]);

            //push the point to the point list
            inputPoints.push_back(point);
        }

        // Parse normals
        else if (parts[0] == "vn") {
            Vec3 normal;
            normal.e[0] = std::stod(parts[1]);
            normal.e[1] = std::stod(parts[2]);
            normal.e[2] = std::stod(parts[3]);

            //push the normal to the normals list
            inputNormals.push_back(normal);
        }

        // Parse texture coords
        else if (parts[0] == "vt") {
            Vec3 texCoord;
            texCoord.e[0] = std::stod(parts[1]);
            texCoord.e[1] = std::stod(parts[2]);

            //push the coord to the texcoords list
            inputTexCoords.push_back(texCoord);
        }

        // Parse faces
        else if (parts[0] == "f") {
            // FIXME: The following statement is incorrect. Faces
            //  do not always have a txtr index, therefore we should
            //  handle that. TBH, we should probably use an external
            //  library altogether for mesh loading...
            // Faces are made up of a vertex index, texture coord
            // index, and also a normal index.
            Vec3 triangle;
            Vec3 normalIndex;
            Vec3 texCoordIndex;

            // Extract all index info from the line
            for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
                auto vertexInfo = splitString(parts[vertexIndex + 1], '/');
                triangle.e[vertexIndex] = std::stod(vertexInfo[0]) - 1;
                if (vertexInfo[1] != "") {
                    texCoordIndex.e[vertexIndex] = std::stod(vertexInfo[1]) - 1;
                }
                normalIndex.e[vertexIndex] = std::stod(vertexInfo[2]) - 1;
            }

            triangleIndices.push_back(triangle);
            normalIndices.push_back(normalIndex);
            texCoordIndices.push_back(texCoordIndex);
        }
    }
    objStream.close();

    // Construct triangle instances from indices and scale vertices
    std::cout << "Reordering Vertices and Creating Triangles" << std::endl;
    for (unsigned int i = 0; i < triangleIndices.size(); i++) {
        auto v0 = std::make_shared<vertex_triangle>();
        auto v1 = std::make_shared<vertex_triangle>();
        auto v2 = std::make_shared<vertex_triangle>();

        // TODO: Multiply these by some scale?
        v0->pos = inputPoints[triangleIndices[i].x()];
        v1->pos = inputPoints[triangleIndices[i].y()];
        v2->pos = inputPoints[triangleIndices[i].z()];

        if (inputNormals.size() != 0) {
            v0->normal = inputNormals[normalIndices[i].x()];
            v1->normal = inputNormals[normalIndices[i].y()];
            v2->normal = inputNormals[normalIndices[i].z()];
        }

        if (inputTexCoords.size() != 0) {
            v0->u = inputTexCoords[texCoordIndices[i].x()].x();
            v0->v = inputTexCoords[texCoordIndices[i].x()].y();
            v1->u = inputTexCoords[texCoordIndices[i].y()].x();
            v1->v = inputTexCoords[texCoordIndices[i].y()].y();
            v2->u = inputTexCoords[texCoordIndices[i].z()].x();
            v2->v = inputTexCoords[texCoordIndices[i].z()].y();
        }

        // Construct a triangle. If normals aren't specified, never use
        // smooth shading (this is a bit of a hack)
        std::shared_ptr<Triangle> tri = std::make_shared<Triangle>(
            v0, v1, v2, defaultMaterial, !inputNormals.empty());
        _triangles->add(tri);
    }

    _hasLoaded = true;
    return true;
}

RAY_NAMESPACE_CLOSE_SCOPE