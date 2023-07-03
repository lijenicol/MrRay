#pragma once
#include "ray/namespace.h"
#include "ray/rtutils.h"
#include "ray/geom/Hittable.h"

RAY_NAMESPACE_OPEN_SCOPE

// Could be a class but it only stores data
struct vertex_triangle {
	Point3 pos;
	Vec3 normal;
	double u;
	double v;
};

// FIXME: Rework this class to grab vertex data from a mesh instance.
//  The updated approach would save memory, and is similar to how PBRT
//  implements triangles.
// Holds information about a triangle such as its vertices,
// Normal and Material, and also provides an intersection method.
class Triangle : public Hittable
{
public:
	std::shared_ptr<vertex_triangle> v0, v1, v2;
	Vec3 normal;
	std::shared_ptr<Material> mat;
	AABB box;
	bool smooth_shading;

	Triangle(std::shared_ptr<vertex_triangle> v0, std::shared_ptr<vertex_triangle> v1, 
		std::shared_ptr<vertex_triangle> v2, std::shared_ptr<Material> mat, bool smooth_shading)
			: mat(mat), v0(v0), v1(v1), v2(v2), smooth_shading(smooth_shading) {
		normal = unit_vector(cross(v1->pos - v0->pos, v2->pos - v0->pos));
		
		// Setup flags
		hasBeenHit = new bool;
		hasBeenTested = new bool;
		*hasBeenHit = false;
		*hasBeenTested = false;

		// Calculate bounding box once 
		Point3 a(
			fmin(fmin(v0->pos.e[0], v1->pos.e[0]), v2->pos.e[0]),
			fmin(fmin(v0->pos.e[1], v1->pos.e[1]), v2->pos.e[1]),
			fmin(fmin(v0->pos.e[2], v1->pos.e[2]), v2->pos.e[2])
		);
		Point3 b(
			fmax(fmax(v0->pos.e[0], v1->pos.e[0]), v2->pos.e[0]),
			fmax(fmax(v0->pos.e[1], v1->pos.e[1]), v2->pos.e[1]),
			fmax(fmax(v0->pos.e[2], v1->pos.e[2]), v2->pos.e[2])
		);
		box = AABB(a, b);
	}

	virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;
	
	// Flags useful for debugging (hacked to work in const functions)
	bool *hasBeenHit;	
	bool *hasBeenTested;
	
private: 
	void get_triangle_uv(const Vec3 &w, double& u, double& v) const;
	Vec3 get_triangle_barycentric(const Vec3& p) const;
	Vec3 get_triangle_normal(const Vec3 &w) const;		
};

RAY_NAMESPACE_CLOSE_SCOPE