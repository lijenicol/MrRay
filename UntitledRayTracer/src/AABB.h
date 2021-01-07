#pragma once
#include "rtutils.h"

class AABB {
	public:
		AABB() {}
		AABB(const Point3& a, const Point3& b) { min = a; max = b; }

		// Intersection test for bounding box
		bool hit(const Ray& r, double t_min, double t_max) const {
			for (int a = 0; a < 3; a++) {
				float invD = 1.f / r.direction()[a];
				float t0 = (min[a] - r.origin()[a]) * invD;
				float t1 = (max[a] - r.origin()[a]) * invD;
				if (invD < 0.f) {
					std::swap(t0, t1);
				}
				t_min = t0 > t_min ? t0 : t_min;
				t_max = t1 < t_max ? t1 : t_max;
				if (t_max <= t_min)
					return false;
			}
			return true;
		}

		Point3 min, max;
};

inline AABB surrounding_box(AABB box0, AABB box1) {
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