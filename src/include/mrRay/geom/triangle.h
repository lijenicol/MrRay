#ifndef MR_RAY_TRIANGLE_H
#define MR_RAY_TRIANGLE_H

#include <tuple>

#include "mrRay/geom/hittable.h"
#include "mrRay/geom/mesh.h"
#include "mrRay/namespace.h"
#include "mrRay/rtutils.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

// Forward declarations
class Mesh;

class Triangle : public Hittable
{
public:
    Triangle(
        int *positionIndex, int *normalIndex, int *uvIndex, Mesh *parentMesh,
        std::shared_ptr<Material> mat);

    virtual bool
    hit(const Ray &r, double t_min, double t_max, hit_record &rec) const override;

    virtual bool
    bounding_box(double time0, double time1, AABB &output_box) const override;

private:
    /// Returns the positions of the triangle vertices
    std::tuple<Vec3, Vec3, Vec3> getVertexPositions() const;
    /// Calculates interpolated u,v coords and stores them in u and v
    void getInterpolatedUV(const Vec3 &w, double &u, double &v) const;
    /// Calculates the barycentric coordinates for a position on the triangle
    Vec3 getTriangleBarycentric(const Vec3 &p) const;
    /// Calculates the interpolated normal
    Vec3 getInterpolatedNormal(const Vec3 &w) const;

    int *_positionIndex;
    int *_normalIndex;
    int *_uvIndex;

    Mesh *_parentMesh;
    std::shared_ptr<Material> _mat;
    Vec3 _normal;
    AABB _boundingBox;
};

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif // MR_RAY_TRIANGLE_H
