#ifndef MR_RAY_AABB_H
#define MR_RAY_AABB_H

#include "mrRay/namespace.h"
#include "mrRay/rtutils.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

class AABB
{
public:
    AABB() {}
    AABB(const Point3 &a, const Point3 &b)
    {
        min = a;
        max = b;
    }

    // Intersection test for bounding box
    bool hit(const Ray &r, double t_min, double t_max) const
    {
        for (int a = 0; a < 3; a++) {
            float invD = 1.f / r.direction()[a];
            float t0 = (min[a] - r.origin()[a]) * invD;
            float t1 = (max[a] - r.origin()[a]) * invD;
            if (invD < 0.f) {
                std::swap(t0, t1);
            }
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max < t_min) return false;
        }
        return true;
    }

    Point3 min, max;
};

AABB surrounding_box(AABB box0, AABB box1);

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif // MR_RAY_AABB_H
