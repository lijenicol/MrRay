#include "BVHNode.h"
#include <algorithm>

void printBVHProgress(int sortedCount, int totalCount) {
	int percent_done = (100.0f * (float)((float)sortedCount / totalCount));
	if (percent_done % 10 == 0) {
		std::cerr << "\rBVH Construction " << percent_done << "% Complete" << std::flush;
	}
}

BVHNode::BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end, double time0, double time1, int depth, int &sorted_counter) {
	// Choose axis for split
	int axis = random_int(0, 2);
	auto comparator = (axis == 0) ? box_x_compare
				    : (axis == 1) ? box_y_compare
			                      : box_z_compare;

	size_t object_span = end - start;

	if (object_span == 1) {
		left = objects[start];
		right = NULL;

		sorted_counter += 1;
		printBVHProgress(sorted_counter, objects.size());
	}
	else if (object_span == 2) {
		// Figure which object goes either side of the split
		if (comparator(objects[start], objects[start + 1])) {
			left = objects[start];
			right = objects[start + 1];
		}
		else {
			left = objects[start + 1];
			right = objects[start];
		}

		sorted_counter += 2;
		printBVHProgress(sorted_counter, objects.size());
	}
	else {
		// Sort objects along axis
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		// Split objects around axis
		size_t mid = start + object_span / 2;
		left = std::make_shared<BVHNode>(objects, start, mid, time0, time1, depth+1, sorted_counter);
		right = std::make_shared<BVHNode>(objects, mid, end, time0, time1, depth+1, sorted_counter);
	}

	// Setup bounding box for the BVH node
	AABB box_left, box_right;
	if (!left->bounding_box(time0, time1, box_left))
		std::cerr << "Bounding box definition missing. Found in BVH node";
	if (right != NULL) {
		if (!right->bounding_box(time0, time1, box_right))
			std::cerr << "Bounding box definition missing. Found in BVH node";
	}
	box = (right!=NULL) ? surrounding_box(box_left, box_right) : box_left;
}

bool BVHNode::bounding_box(double time0, double time1, AABB& output_box) const {
	output_box = box;
	return true;
}

bool BVHNode::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
	if (!box.hit(r, t_min, t_max))
		return false;

	bool hit_left = left->hit(r, t_min, t_max, rec);
	bool hit_right = (right!=NULL) ? right->hit(r, t_min, hit_left ? rec.t : t_max, rec) : false;

	return hit_left || hit_right;
}

bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis) {
	AABB box_a;
	AABB box_b;
	if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
		std::cerr << "Missing bounding box implementation.\n";
	return box_a.min.e[axis] < box_b.min.e[axis];
}

bool box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
	return box_compare(a, b, 0);
}

bool box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
	return box_compare(a, b, 1);
}

bool box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
	return box_compare(a, b, 2);
}