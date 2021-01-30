#include "Transform.h"

//
// TRANSLATION METHODS
//

bool Translate::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
	// Move ray by the inverse offset and test intersection
	Ray moved_r(r.origin() - offset, r.direction());
	if (!obj->hit(moved_r, t_min, t_max, rec))
		return false;

	// Set the true hit point in the rec
	rec.p += offset;
	rec.set_face_normal(moved_r, rec.normal);

	return true;
}

bool Translate::bounding_box(double time0, double time1, AABB& output_box) const {
	// Get bounding box of object instance
	if (!obj->bounding_box(time0, time1, output_box))
		return false;

	// Move bounding box
	output_box = AABB(
		output_box.min + offset,
		output_box.max + offset);

	return true;
}