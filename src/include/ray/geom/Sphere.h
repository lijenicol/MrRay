#pragma once
#include "ray/namespace.h"
#include "ray/rtutils.h"
#include "ray/geom/Hittable.h"

RAY_NAMESPACE_OPEN_SCOPE

class Sphere : public Hittable
{
	public :
		Point3 center;
		double radius;
		std::shared_ptr<Material> mat;

		Sphere(Point3 center, double radius, std::shared_ptr<Material> m) : center(center), radius(radius), mat(m) {}
		virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
		virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;
		static void get_sphere_uv(const Vec3& p, double& u, double& v);
};

RAY_NAMESPACE_CLOSE_SCOPE
