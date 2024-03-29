#ifndef MR_RAY_TRANSFORM_H
#define MR_RAY_TRANSFORM_H

#include "mrRay/geom/hittable.h"
#include "mrRay/namespace.h"
#include "mrRay/rtutils.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

class Transform : public Hittable
{
public:
    Transform(
        std::shared_ptr<Hittable> obj, Vec3 offset, Vec3 rotation, Vec3 scale)
        : obj(obj)
        , offset(offset)
        , rotation(rotation)
        , scale(scale)
    {
    }

    virtual bool
    hit(const Ray &r, double t_min, double t_max, hit_record &rec) const override;

    virtual bool
    bounding_box(double time0, double time1, AABB &output_box) const override;

private:
    std::shared_ptr<Hittable> obj;
    Vec3 offset;
    Vec3 rotation;
    Vec3 scale;
};

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif // MR_RAY_TRANSFORM_H
