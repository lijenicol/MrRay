//
// Created by lijenicol on 25/06/23.
//

#ifndef RAY_OBJLOADER_H
#define RAY_OBJLOADER_H

#include "ray/namespace.h"
#include "ray/geom/HittableList.h"
#include "ray/geom/Triangle.h"

RAY_NAMESPACE_OPEN_SCOPE

/// Abstract class for loading a set of triangles for a mesh
class MeshLoader
{
public:
    MeshLoader(const std::string& meshPath);
    /// Loads the triangles that represent the mesh
    ///
    /// \return Whether the load was successful
    virtual bool loadTriangles() = 0;
    /// Returns the triangles that were loaded
    HittableList* getTriangles();
protected:
    const std::string _meshPath;
    bool _hasLoaded;
    std::unique_ptr<HittableList> _triangles;
};

/// Loader for OBJ files
class OBJLoader final: public MeshLoader
{
public:
    OBJLoader(const std::string& meshPath);
    bool loadTriangles() override;
};

RAY_NAMESPACE_CLOSE_SCOPE

#endif //RAY_OBJLOADER_H
