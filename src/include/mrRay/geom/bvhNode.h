#pragma once
#include "mrRay/namespace.h"
#include "mrRay/rtutils.h"
#include "mrRay/geom/hittable.h"
#include "mrRay/geom/hittableList.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

class BVHNode : public Hittable {
public:
	BVHNode(HittableList& list, double time0, double time1) {
		std::cerr << "\n";
		sorted_counter_master = 0;
		BVHNode container = BVHNode(list.objects, 0, list.objects.size(), time0, time1, 0, sorted_counter_master);
		box = container.box;
		left = container.left;
		right = container.right;
		std::cerr << "\rBVH Construction 100% Complete" << std::flush;
		std::cerr << "\n";
	}
	BVHNode(std::vector<std::shared_ptr<Hittable>>& src_objects, size_t start, size_t end, double time0, double time1, int depth, int &sorted_counter);

	virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

	std::shared_ptr<Hittable> left;
	std::shared_ptr<Hittable> right;
	AABB box;
	int sorted_counter_master;
};

//bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis);
bool box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
bool box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
bool box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);

MR_RAY_NAMESPACE_CLOSE_SCOPE