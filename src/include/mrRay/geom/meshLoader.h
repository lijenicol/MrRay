//
// Created by lijenicol on 25/06/23.
//

#ifndef MR_RAY_OBJLOADER_H
#define MR_RAY_OBJLOADER_H

#include "mrRay/namespace.h"
#include "mrRay/geom/HittableList.h"
#include "mrRay/geom/Triangle.h"
#include "mrRay/geom/mesh.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

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

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif //MR_RAY_OBJLOADER_H
