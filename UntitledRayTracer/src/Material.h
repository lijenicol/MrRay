#pragma once
#include "rtutils.h"
#include "PDF.h"

struct hit_record;

struct scatter_record {
	Ray specularRay;
	bool isSpecular;
	Colour attenuation;
	std::shared_ptr<PDF> PDF_ptr;
};

class Material {
public:
	virtual bool scatter(const Ray& r_in, const hit_record& rec, scatter_record& srec) const {
		return false;
	}

	virtual double bsdf(const Ray& r_in, const hit_record& rec, const Ray& r_out) const {
		return 0;
	}

	virtual Colour emitted(double u, double v, const Point3& p) const {
		return Colour(0, 0, 0);
	}
};

// Schlick approximation
inline double schlick(double cosine, double ref_idx) {
	double r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow(1 - cosine,5);
}