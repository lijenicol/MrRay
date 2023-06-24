#pragma once
#include "ray/namespace.h"
#include "ray/rtutils.h"
#include "ray/geom/Hittable.h"

RAY_NAMESPACE_OPEN_SCOPE

class Disk : public Hittable {
	public:
		Point3 center;
		double radius, innerRadius;
		std::shared_ptr<Material> mat;

		Disk(Point3 center, double radius, double innerRadius, std::shared_ptr<Material> m) 
			: center(center), radius(radius), innerRadius(innerRadius), mat(m) {}
		virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
		virtual bool bounding_box(double time0, double time1, AABB& output_box) const override {
			output_box = AABB(Point3(center[0] - radius, center[1]-0.0001, center[2] - radius), Point3(center[0] + radius, center[1] + 0.0001, center[2] + radius));
			return true;
		}
};

RAY_NAMESPACE_CLOSE_SCOPE
