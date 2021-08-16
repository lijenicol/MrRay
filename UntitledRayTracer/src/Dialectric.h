#pragma once
#include "rtutils.h"
#include "Material.h"

class Dialectric : public Material {
public: 
	double ref_idx;
	Dialectric(double ri) : ref_idx(ri) {}

	virtual bool scatter(const Ray& r_in, const hit_record& rec, scatter_record& srec) const override {
		srec.attenuation = Colour(1, 1, 1);
		srec.isSpecular = true;

		// This value is the ratio of the refractive indexes (ri_in / ri_out)
		// We use ri_in = 1 as a good approximation for air
		double etai_over_etat = rec.front_face ? (1 / ref_idx) : ref_idx;
		Vec3 unit_direction = unit_vector(r_in.direction());

		// Calculate total internal reflection
		// (dot with -unit_dir to get angle < 90deg)
		double cos_theta = dot(-unit_direction, unit_vector(rec.normal));
		double sin_theta = sqrt(1 - cos_theta * cos_theta);
		if (etai_over_etat * sin_theta > 1) {
			Vec3 reflected = reflect(unit_direction, rec.normal);
			srec.specularRay = Ray(rec.p, reflected);
			return true;
		}

		// Use schlick approximation to get a reflected ray
		double reflect_prob = schlick(cos_theta, ref_idx);
		if (random_double() < reflect_prob) {
			Vec3 reflected = reflect(unit_direction, rec.normal);
			srec.specularRay = Ray(rec.p, reflected);
			return true;
		}

		// If not reflected then refract
		Vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
		srec.specularRay = Ray(rec.p, refracted);
		return true;
	}
};