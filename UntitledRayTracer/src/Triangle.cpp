#include "Triangle.h"

bool Triangle::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
	// Check first bounding box intersection test. This is to speed up calculations (hypothetically)
	// May remove in the future because having triangles in a BVH will automatically cure this problem
	// However, for now it does the job
	if (!box.hit(r, t_min, t_max))
		return false;

	//return if ray is parallel with triangle
	if (dot(normal, r.direction()) == 0)
		return false;

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

	// Set hit record
	rec.t = t;
	rec.p = intersectionPoint;
	rec.set_face_normal(r, normal);
	get_triangle_uv(rec.p, rec.u, rec.v);
	rec.mat = mat;

	return true;
}

void Triangle::get_triangle_uv(const Vec3& p, double& u, double& v) const {
	// Get triangle areas
	double total_area_2 = cross(v1->pos - v0->pos, v2->pos - v0->pos).length();
	double a_area_2 = cross(p - v2->pos, p - v1->pos).length();
	double b_area_2 = cross(p-v0->pos, p-v2->pos).length();
	double c_area_2 = cross(p-v1->pos, p-v0->pos).length();

	// Compute barycentric
	double wa = a_area_2 / total_area_2;
	double wb = b_area_2 / total_area_2;
	double wc = c_area_2 / total_area_2;

	// Compute u,v coords
	u = v0->u * wa + v1->u * wb + v2->u * wc;
	v = v0->v * wa + v1->v * wb + v2->v * wc;
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