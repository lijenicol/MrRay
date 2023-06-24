#include "ray/geom/Transform.h"

RAY_NAMESPACE_OPEN_SCOPE

//
// TRANSFORM METHODS
//

// Rotate a point around the x-axis
Vec3 rotatePointX(Vec3 point, double rx) {
	auto newy = point.y();
	auto newz = point.z();

	newy = cos(rx) * point.y() - sin(rx) * point.z();
	newz = sin(rx) * point.y() + cos(rx) * point.z();

	return Vec3(point.x(), newy, newz);
}

// Rotate a point around the y-axis
Vec3 rotatePointY(Vec3 point, double ry) {
	auto newx = point.x();
	auto newz = point.z();

	newx =  cos(ry) * point.x() + sin(ry) * point.z();
	newz = -sin(ry) * point.x() + cos(ry) * point.z();

	return Vec3(newx, point.y(), newz);
}

// Rotate a point around the z-axis
Vec3 rotatePointZ(Vec3 point, double rz) {
	auto newx = point.x();
	auto newy = point.y();

	newx = cos(rz) * point.x() - sin(rz) * point.y();
	newy = sin(rz) * point.x() + cos(rz) * point.y();

	return Vec3(newx, newy, point.z());
}

Vec3 rotatePoint(Vec3 point, Vec3 rotation) {
	// Get rotation x, y, z in rads
	auto rx = degrees_to_radians(rotation.x());
	auto ry = degrees_to_radians(rotation.y());
	auto rz = degrees_to_radians(rotation.z());

	return rotatePointZ(
		rotatePointY(
			rotatePointX(point, rx), 
			ry
		), 
		rz
	);
}

Vec3 rotatePointInverse(Vec3 point, Vec3 rotation) {
	// Get rotation x, y, z in rads
	auto rx = degrees_to_radians(rotation.x());
	auto ry = degrees_to_radians(rotation.y());
	auto rz = degrees_to_radians(rotation.z());

	return rotatePointX(
		rotatePointY(
			rotatePointZ(point, -rz),
			-ry
		),
		-rx
	);
}

bool Transform::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
	// Move ray, rotate ray, scale ray
	Point3 origin = r.origin();
	Vec3 direction = r.direction();

	// Move ray
	origin = origin - offset;

	// Rotate ray
	direction = rotatePointInverse(direction, rotation);
	origin = rotatePointInverse(origin, rotation);

	// Scale ray
	direction = unit_vector(direction / scale);
	origin = origin / scale;

	Ray transformedRay(origin, direction);

	// Test intersection
	if (!obj->hit(transformedRay, t_min, t_max, rec))
		return false;

	// Unscale, unrotate, unmove
	Point3 p = rec.p;
	Vec3 normal = rec.normal;

	// Unscale
	p = p * scale;
	normal = unit_vector(normal / scale);

	// Unrotate
	p = rotatePoint(p, rotation);
	normal = rotatePoint(normal, rotation);

	// Unmove
	p = p + offset;

	// Set the true hit point in the rec
	rec.p = p;

	// Set correct normal
	rec.set_face_normal(r, normal);
	
	// Make sure that rec t is correct 
	rec.t = (rec.p - r.origin()).length();

	return true;
}

bool Transform::bounding_box(double time0, double time1, AABB& output_box) const {
	// Get bounding box of object instance
	if (!obj->bounding_box(time0, time1, output_box))
		return false;
	
	// Calculate new bounding box
	Point3 min(infinity, infinity, infinity);
	Point3 max(-infinity, -infinity, -infinity);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				auto x = i * output_box.max.x() + (1.0 - i) * output_box.min.x();
				auto y = j * output_box.max.y() + (1.0 - j) * output_box.min.y();
				auto z = k * output_box.max.z() + (1.0 - k) * output_box.min.z();

				// Rotate the point for testing
				Vec3 tester = rotatePoint(Vec3(x, y, z), rotation);

				// Update bounding box
				for (int c = 0; c < 3; c++) {
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}
		}
	}
	
	// Scale and move bounding box
	output_box = AABB(
		min * scale + offset,
		max * scale + offset
	);

	return true;
}

RAY_NAMESPACE_CLOSE_SCOPE
