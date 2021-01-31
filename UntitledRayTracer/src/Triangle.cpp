#include "Triangle.h"

bool Triangle::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
	// Check first bounding box intersection test. This is to speed up calculations (hypothetically)
	// May remove in the future because having triangles in a BVH will automatically cure this problem
	// However, for now it does the job
	/*if (!box.hit(r, t_min, t_max))
		return false;*/

	//return if ray is parallel with triangle
	if (dot(normal, r.direction()) == 0) {
		return false;
	}

	//calculate plane intersection
	float d = dot(normal, v0->pos);
	float t = (d - dot(normal, r.origin())) / dot(normal, r.direction());
	Point3 intersectionPoint = r.origin() + t * r.direction();

	// Don't report hits outside of the range (or else spooky stuff happens)
	if (t > t_max || t < t_min)
		return false;

	//calculate if intersection point is within triangle
	float testOne = dot(cross(v1->pos - v0->pos,
		intersectionPoint - v0->pos),
		normal);
	if (testOne < 0)
		return false;

	float testTwo = dot(cross(v2->pos - v1->pos,
		intersectionPoint - v1->pos),
		normal);
	if (testTwo < 0)
		return false;

	float testThree = dot(cross(v0->pos - v2->pos,
		intersectionPoint - v2->pos),
		normal);
	if (testThree < 0)
		return false;

	// Compute barycentric weights
	Vec3 w;
	w = get_triangle_barycentric(rec.p);

	// Set hit record
	rec.t = t;
	rec.p = intersectionPoint;
	get_triangle_uv(rec.p, rec.u, rec.v);
	rec.mat = mat;

	// If smooth shading then use interpolated normals
	if (smooth_shading) {
		Vec3 _normal = get_triangle_normal(w);
		rec.set_face_normal(r, _normal);
	}
	else {
		rec.set_face_normal(r, normal);
	}

	return true;
}

Vec3 Triangle::get_triangle_barycentric(const Vec3& p) const {
	// Get triangle areas
	double total_area_2 = cross(v1->pos - v0->pos, v2->pos - v0->pos).length();
	double a_area_2 = cross(p - v2->pos, p - v1->pos).length();
	double b_area_2 = cross(p - v0->pos, p - v2->pos).length();
	double c_area_2 = cross(p - v1->pos, p - v0->pos).length();

	// Compute barycentric
	double wa = a_area_2 / total_area_2;
	double wb = b_area_2 / total_area_2;
	double wc = c_area_2 / total_area_2;

	return Vec3(wa, wb, wc);
}

// Takes in the already pre-computed barycentric coordinates of the point in question
void Triangle::get_triangle_uv(const Vec3& w, double& u, double& v) const {
	// Compute interpolated u,v coords
	u = v0->u * w[0] + v1->u * w[1] + v2->u * w[2];
	v = v0->v * w[0] + v1->v * w[1] + v2->v * w[2];
}

Vec3 Triangle::get_triangle_normal(const Vec3& w) const {
	// Compute interpolated normal
	double x = v0->normal[0] * w[0] + v1->normal[0] * w[1] + v2->normal[0] * w[2];
	double y = v0->normal[1] * w[0] + v1->normal[1] * w[1] + v2->normal[1] * w[2];
	double z = v0->normal[2] * w[0] + v1->normal[2] * w[1] + v2->normal[2] * w[2];
	return unit_vector(Vec3(x,y,z));
}

bool Triangle::bounding_box(double time0, double time1, AABB& output_box) const {
	// calculate min/max x
	/*Point3 a(
		fmin(fmin(v0->pos.e[0], v1->pos.e[0]), v2->pos.e[0]),
		fmin(fmin(v0->pos.e[1], v1->pos.e[1]), v2->pos.e[1]),
		fmin(fmin(v0->pos.e[2], v1->pos.e[2]), v2->pos.e[2])
	);
	Point3 b(
		fmax(fmax(v0->pos.e[0], v1->pos.e[0]), v2->pos.e[0]),
		fmax(fmax(v0->pos.e[1], v1->pos.e[1]), v2->pos.e[1]),
		fmax(fmax(v0->pos.e[2], v1->pos.e[2]), v2->pos.e[2])
	);*/

	output_box = box;
	return true;
}