#pragma once
#include "rtutils.h"
#include "Hittable.h"

class Disk : public Hittable {
	public:
		Point3 center;
		double radius, innerRadius;
		std::shared_ptr<Material> mat;

		Disk(Point3 center, double radius, double innerRadius, std::shared_ptr<Material> m) 
			: center(center), radius(radius), innerRadius(innerRadius), mat(m) {}
		virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
};