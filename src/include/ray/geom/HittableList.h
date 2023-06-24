#pragma once
#include "ray/namespace.h"
#include "ray/geom/Hittable.h"

#include <vector>
#include <memory>

RAY_NAMESPACE_OPEN_SCOPE

class HittableList : public Hittable {
public:
	std::vector<std::shared_ptr<Hittable>> objects;

	HittableList() {}
	HittableList(std::shared_ptr<Hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

	virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;
};

RAY_NAMESPACE_CLOSE_SCOPE
