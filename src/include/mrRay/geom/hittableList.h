#ifndef MR_RAY_HITTABLELIST_H
#define MR_RAY_HITTABLELIST_H

#include "mrRay/geom/hittable.h"
#include "mrRay/namespace.h"

#include <memory>
#include <vector>

MR_RAY_NAMESPACE_OPEN_SCOPE

class HittableList : public Hittable
{
public:
    std::vector<std::shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

    virtual bool
    hit(const Ray &r, double t_min, double t_max, hit_record &rec) const override;

    virtual bool
    bounding_box(double time0, double time1, AABB &output_box) const override;
};

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif // MR_RAY_HITTABLELIST_H
