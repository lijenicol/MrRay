#pragma once
#include "Hittable.h"

#include <vector>
#include <memory>

class HittableList : public Hittable {
public:
	std::vector<std::shared_ptr<Hittable>> objects;

	HittableList() {}
	HittableList(std::shared_ptr<Hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

	virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
};