//
// Created by lijenicol on 25/06/23.
//

#include <fstream>

#include "mrRay/geom/meshLoader.h"
#include "mrRay/material/material.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

MeshLoader::MeshLoader(const std::string &meshPath)
    : _meshPath(meshPath)
    , _hasLoaded(false)
{
    _loadedMeshInfo = new RawMeshInfo;
}

MeshLoader::~MeshLoader()
{
    delete _loadedMeshInfo;
}

RawMeshInfo *
MeshLoader::getMeshInfo() const
{
    if (_hasLoaded) {
        return _loadedMeshInfo;
    }
    return nullptr;
}

/// Split a string into multiple pieces at delimiter points
std::vector<std::string>
splitString(std::string s, char delimiter)
{
    std::vector<std::string> splits;

    // Find occurrence of delimiter and split
    size_t pos;
    while ((pos = s.find(delimiter)) != std::string::npos) {
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
OBJLoader::load()
{
    std::ifstream objStream(_meshPath);
    if (!objStream.is_open()) {
        std::cerr << "File cannot be read or does not exist: " << _meshPath
                  << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(objStream, line)) {
        // Split string
        auto parts = splitString(line, ' ');

        // Parse vertices
        if (parts[0] == "v") {
            Vec3 point;
            point.e[0] = std::stod(parts[1]);
            point.e[1] = std::stod(parts[2]);
            point.e[2] = std::stod(parts[3]);
            _loadedMeshInfo->positions.push_back(point);
        }

        // Parse normals
        else if (parts[0] == "vn")
        {
            Vec3 normal;
            normal.e[0] = std::stod(parts[1]);
            normal.e[1] = std::stod(parts[2]);
            normal.e[2] = std::stod(parts[3]);
            _loadedMeshInfo->normals.push_back(normal);
        }

        // Parse texture coords
        else if (parts[0] == "vt")
        {
            Vec3 texCoord;
            texCoord.e[0] = std::stod(parts[1]);
            texCoord.e[1] = std::stod(parts[2]);
            _loadedMeshInfo->uvs.push_back(texCoord);
        }

        // Parse faces
        else if (parts[0] == "f")
        {
            for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
                auto vertexInfo = splitString(parts[vertexIndex + 1], '/');
                _loadedMeshInfo->positionIndices.push_back(
                    std::stoi(vertexInfo[0]) - 1);
                if (vertexInfo[1] != "") {
                    _loadedMeshInfo->uvIndices.push_back(
                        std::stoi(vertexInfo[1]) - 1);
                }
                _loadedMeshInfo->normalIndices.push_back(
                    std::stoi(vertexInfo[2]) - 1);
            }
        }
    }
    objStream.close();

    _hasLoaded = true;
    return true;
}

MR_RAY_NAMESPACE_CLOSE_SCOPE