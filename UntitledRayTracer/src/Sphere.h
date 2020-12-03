#pragma once
#include "rtutils.h"
#include "Hittable.h"

class Sphere : public Hittable
{
	public :
		Point3 center;
		double radius;
		std::shared_ptr<Material> mat;

		Sphere(Point3 center, double radius, std::shared_ptr<Material> m) : center(center), radius(radius), mat(m) {}
		virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
		static void get_sphere_uv(const Vec3& p, double& u, double& v);
};

