#pragma once
#include "../rtutils.h"
#include "Material.h"

class Metal : public Material {
public:
	Colour albedo;
	double fuzz;
	Metal(const	Colour& a, double fuzz) : albedo(a), fuzz(fuzz < 1.0 ? fuzz : 1.0) {}

	// TODO: could probably rewrite this class to be a combination 
	// 	   of Lambertian and a mirror
	// This is a mixture between diffuse and specular
	virtual bool scatter(const Ray& r_in, const hit_record& rec, scatter_record& srec) const override {
		srec.attenuation = albedo;
		
		// We gonna split between diffuse and specular
		double prob = random_double();
		if (prob > fuzz) {
			// Specular
			srec.isSpecular = true;
			Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
			srec.specularRay = Ray(rec.p, reflected);
		}
		else {
			// Diffuse
			srec.isSpecular = false;
			srec.PDF_ptr = std::make_shared<CosinePDF>(rec.normal);
		}

		//scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		return true;
	}

	virtual double bsdf(const Ray& r_in, const hit_record& rec, const Ray& r_out) const override {
		return 1 / pi;
	}
};