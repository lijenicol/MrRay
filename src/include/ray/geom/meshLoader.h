//
// Created by lijenicol on 25/06/23.
//

#ifndef RAY_OBJLOADER_H
#define RAY_OBJLOADER_H

#include "ray/namespace.h"
#include "ray/geom/HittableList.h"
#include "ray/geom/Triangle.h"
#include "ray/geom/mesh.h"

RAY_NAMESPACE_OPEN_SCOPE

/// Abstract class for loading a set of triangles for a mesh
class MeshLoader
{
public:
    MeshLoader(const std::string& meshPath);
    ~MeshLoader();
    /// Loads all info that represent the mesh
    ///
    /// \return Whether the load was successful
    virtual bool load() = 0;
    /// Returns the loaded mesh info
    RawMeshInfo* getMeshInfo() const;
    /// Returns whether the mesh has been loaded
    bool hasLoaded() const { return _hasLoaded; }

protected:
    const std::string _meshPath;
    bool _hasLoaded;
    RawMeshInfo* _loadedMeshInfo;
};

/// Loader for OBJ files
class OBJLoader final: public MeshLoader
{
public:
    OBJLoader(const std::string& meshPath);
    bool load() override;
};

RAY_NAMESPACE_CLOSE_SCOPE

#endif //RAY_OBJLOADER_H
