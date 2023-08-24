#pragma once
#include "mrRay/geom/hittable.h"
#include "mrRay/namespace.h"
#include "mrRay/rtutils.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

class XYRect : public Hittable
{
public:
    std::shared_ptr<Material> mat;
    double x0, x1, y0, y1, k;

    XYRect(
        double x0, double x1, double y0, double y1, double k,
        std::shared_ptr<Material> mat)
        : x0(x0)
        , x1(x1)
        , y0(y0)
        , y1(y1)
        , k(k)
        , mat(mat)
    {
    }

    virtual bool
    hit(const Ray &r, double t_min, double t_max, hit_record &rec) const override;

    virtual bool
    bounding_box(double time0, double time1, AABB &output_box) const override
    {
        output_box = AABB(Point3(x0, y0, k - 0.0001), Point3(x1, y1, k + 0.0001));
        return true;
    }
};

class XZRect : public Hittable
{
public:
    std::shared_ptr<Material> mat;
    double x0, x1, z0, z1, k;

    XZRect(
        double x0, double x1, double y0, double y1, double k,
        std::shared_ptr<Material> mat)
        : x0(x0)
        , x1(x1)
        , z0(y0)
        , z1(y1)
        , k(k)
        , mat(mat)
    {
    }

    virtual bool
    hit(const Ray &r, double t_min, double t_max, hit_record &rec) const override;

    virtual bool
    bounding_box(double time0, double time1, AABB &output_box) const override
    {
        output_box = AABB(Point3(x0, k - 0.0001, z0), Point3(x1, k + 0.0001, z1));
        return true;
    }

    // Given a random direction, what is the PDF of sampling this object
    virtual double
    pdf_value(const Point3 &o, const Vec3 &direction) const override
    {
        // Trace a ray to this hittable from the given location and direction
        hit_record rec;
        if (!this->hit(Ray(o, direction), 0.001, infinity, rec)) {
            return 0;
        }

        double area = (x1 - x0) * (z1 - z0);
        double distanceSquared = rec.t * rec.t * direction.length_squared();
        double cosine = fabs(dot(direction, rec.normal) / direction.length());

        return distanceSquared / (cosine * area);
    }

    // Generate a random direction to sample this PDF from
    virtual Vec3 random(const Vec3 &o) const override
    {
        Point3 randomPoint
            = Point3(random_double(x0, x1), k, random_double(z0, z1));
        return unit_vector(randomPoint - o);
    }
};

class YZRect : public Hittable
{
public:
    std::shared_ptr<Material> mat;
    double y0, y1, z0, z1, k;

    YZRect(
        double x0, double x1, double y0, double y1, double k,
        std::shared_ptr<Material> mat)
        : y0(x0)
        , y1(x1)
        , z0(y0)
        , z1(y1)
        , k(k)
        , mat(mat)
    {
    }

    virtual bool
    hit(const Ray &r, double t_min, double t_max, hit_record &rec) const override;

    virtual bool
    bounding_box(double time0, double time1, AABB &output_box) const override
    {
        output_box = AABB(Point3(k - 0.0001, y0, z0), Point3(k + 0.0001, y1, z1));
        return true;
    }
};

MR_RAY_NAMESPACE_CLOSE_SCOPE
