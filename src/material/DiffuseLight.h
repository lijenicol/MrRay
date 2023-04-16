#pragma once
#include "../rtutils.h"
#include "Material.h"

class DiffuseLight : public Material {
public:
	Colour albedo;
	DiffuseLight(const Colour& a) : albedo(a) {}

	virtual bool scatter(const Ray& r_in, const hit_record& rec, scatter_record& srec) const override {
		return false;
	}

	virtual Colour emitted(double u, double v, const Point3& p) const override {
		return albedo;
	}
};