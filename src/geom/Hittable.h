#pragma once

#include "../rtutils.h"
#include "../AABB.h"

class Material;

// A hit record contains information about a ray intersection with an object
struct hit_record {
	Point3 p;
	Vec3 normal;
	Material* mat;
	double t;
	double u;
	double v;
	bool front_face;

	inline void set_face_normal(const Ray& r, const Vec3& outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class Hittable {
public : 
	virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(double time0, double time1, AABB& output_box) const = 0;

	// Given a random direction, what is the PDF of sampling this object
	virtual double pdf_value(const Point3& o, const Vec3& direction) const {
		return 0.0;
	}

	// Generate a random direction to sample this PDF from
	virtual Vec3 random(const Vec3& o) const {
		return Vec3(1, 0, 0);
	}
};