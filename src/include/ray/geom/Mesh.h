/**
* A mesh is a collection of triangles. This class loads in a mesh from a .obj file and sets
* up all the triangle instances needed such that the mesh can be rendered. 
 **/

#pragma once
#include "ray/namespace.h"
#include "ray/rtutils.h"
#include "ray/geom/Hittable.h"
#include "ray/geom/BVHNode.h"
#include "ray/geom/Triangle.h"

RAY_NAMESPACE_OPEN_SCOPE

class Mesh : public Hittable {
public:
	Hittable* container;
	std::vector<std::shared_ptr<Triangle>> triangles;
	bool smooth_shading;
	std::shared_ptr<Material> mat;
	double scale;

	Mesh(std::string filename, double scale, const bool smooth_shading, std::shared_ptr<Material> mat);
	void print(bool verbose);
	void printHitTriangles();

	virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

private:
	int vertexCount, faceCount;
};

RAY_NAMESPACE_CLOSE_SCOPE