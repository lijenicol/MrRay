#include "Disk.h"

bool Disk::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
	// Compute ray intersection with plane
	double t = (center.y() - r.origin().y()) / r.dir.y();
	if (t > t_max || t < t_min) {
		return false;
	}

	// Test to see in bounds
	Point3 hitPoint = r.at(t);
	float dist2 = (hitPoint.x() - center.x()) * (hitPoint.x() - center.x()) 
		+ (hitPoint.z() - center.z()) * (hitPoint.z() - center.z());
	if (dist2 > radius * radius || dist2 < innerRadius * innerRadius) {
		return false;
	}
	rec.t = t;
	rec.p = hitPoint;

	// Compute u,v coordinates
	float u = 1 / (2 * pi) * atan2(hitPoint.z(), hitPoint.x());
	rec.u = u < 0 ? u + 1 : u;
	float dist = sqrt(dist2);
	float vinv = (dist - innerRadius) / (radius - innerRadius);
	rec.v = 1 - vinv;

	// Set normal and material
	rec.set_face_normal(r, Vec3(0, 1, 0));
	rec.mat = mat;
	return true;
}