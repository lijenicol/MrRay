#pragma once
#include "ray/namespace.h"
#include "ray/rtutils.h"
#include "ray/geom/Hittable.h"

RAY_NAMESPACE_OPEN_SCOPE

class Transform : public Hittable {
	public:
		Transform(std::shared_ptr<Hittable> obj, Vec3 offset, Vec3 rotation, Vec3 scale)
			: obj(obj), offset(offset), rotation(rotation), scale(scale) {}

		virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
		virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

	private:
		std::shared_ptr<Hittable> obj;
		Vec3 offset;
		Vec3 rotation;
		Vec3 scale;
};

RAY_NAMESPACE_CLOSE_SCOPE
