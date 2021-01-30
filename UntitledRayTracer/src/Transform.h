#pragma once
#include "rtutils.h"
#include "Hittable.h"

class Translate : public Hittable {
	public:
		Translate(std::shared_ptr<Hittable> obj, Vec3 offset)
			: obj(obj), offset(offset) {}

		virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
		virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

	private:
		std::shared_ptr<Hittable> obj;
		Vec3 offset;
};