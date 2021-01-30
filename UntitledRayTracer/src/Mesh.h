/**
* A mesh is a collection of triangles. This class loads in a mesh from a .obj file and sets
* up all the triangle instances needed such that the mesh can be rendered. 
 **/

#pragma once
#include "rtutils.h"
#include "Hittable.h"
#include "BVHNode.h"

class Mesh : public Hittable {
	public:
		BVHNode* container;
		bool smooth_shading;
		std::shared_ptr<Material> mat;

		Mesh(std::string filename, const bool smooth_shading, std::shared_ptr<Material> mat);

		virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
		virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;
};