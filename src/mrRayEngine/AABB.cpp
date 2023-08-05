#include "mrRay/AABB.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

AABB surrounding_box(AABB box0, AABB box1) {
	Point3 a(
		fmin(box0.min[0], box1.min[0]),
		fmin(box0.min[1], box1.min[1]),
		fmin(box0.min[2], box1.min[2])
	);
	Point3 b(
		fmax(box0.max[0], box1.max[0]),
		fmax(box0.max[1], box1.max[1]),
		fmax(box0.max[2], box1.max[2])
	);
	return AABB(a, b);
}

MR_RAY_NAMESPACE_CLOSE_SCOPE
