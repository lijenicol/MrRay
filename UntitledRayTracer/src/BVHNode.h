#pragma once
#include "rtutils.h"
#include "Hittable.h"
#include "HittableList.h"

class BVHNode : public Hittable {
public:
	BVHNode(){}
	BVHNode(const HittableList& list, double time0, double time1)
		: BVHNode(list.objects, 0, list.objects.size(), time0, time1) {std::cerr << "Creating BVH.\n";}
	BVHNode(const std::vector<std::shared_ptr<Hittable>>& src_objects, size_t start, size_t end, double time0, double time1);

	virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

	std::shared_ptr<Hittable> left;
	std::shared_ptr<Hittable> right;
	AABB box;
};

bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis);
bool box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
bool box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
bool box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);