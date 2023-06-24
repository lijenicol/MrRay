#include "ray/geom/Sphere.h"
#include <iostream>

RAY_NAMESPACE_OPEN_SCOPE

bool Sphere::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
	Vec3 oc = r.origin() - center;
	
	// Calculate quadratic discriminant
	double a = dot(r.direction(), r.direction());
	double b = 2.0 * dot(oc, r.direction());
	double c = dot(oc, oc) - radius * radius;
	double discriminant = (b * b) - (4 * a * c);

	// If discriminant > 0, then we have two solutions
	// Solve both solutions and change hit record accordingly
	if (discriminant > 0) {
		double root = sqrt(discriminant);

		double temp = (-b - root) / (2 * a);
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.at(rec.t);
			Vec3 outward_normal = (rec.p - center) / radius;  // Unit normal
			rec.set_face_normal(r, outward_normal);
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
			rec.mat = mat.get();
			return true;
		}

		temp = (-b + root) / (2 * a);
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.at(rec.t);
			Vec3 outward_normal = (rec.p - center) / radius;  // Unit normal
			rec.set_face_normal(r, outward_normal);
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
			rec.mat = mat.get();
			return true;
		}
	}

	// No solutions (discriminant < 0)
	return false;
}

void Sphere::get_sphere_uv(const Vec3& p, double& u, double& v) {
	double phi = atan2(p.z(), p.x());
	double theta = asin(p.y());
	u = 1 - (phi + pi) / (2 * pi);
	v = (theta + pi / 2) / pi;
}

bool Sphere::bounding_box(double time0, double time1, AABB& output_box) const {
	output_box = AABB(
		center - Vec3(radius, radius, radius),
		center + Vec3(radius, radius, radius)
	);
	return true;
}

RAY_NAMESPACE_CLOSE_SCOPE
