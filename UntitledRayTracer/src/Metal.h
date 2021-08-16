#pragma once
#include "rtutils.h"
#include "Material.h"

class Metal : public Material {
public:
	Colour albedo;
	double fuzz;
	Metal(const	Colour& a, double fuzz) : albedo(a), fuzz(fuzz < 1.0 ? fuzz : 1.0) {}

	// TODO: implement this method properly given new BSDF framework
	virtual bool scatter(const Ray& r_in, const hit_record& rec, scatter_record& srec) const override {
		Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		//scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		//srec.attenuation = albedo;
		//return dot(scattered.direction(), rec.normal) > 0;
		return false;
	}
};