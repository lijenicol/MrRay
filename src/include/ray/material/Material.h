#ifndef MATERIAL_MATERIAL_H
#define MATERIAL_MATERIAL_H

#include "ray/namespace.h"
#include "ray/memory.h"
#include "ray/PDF.h"
#include "ray/rtutils.h"
#include "ray/material/Texture.h"

RAY_NAMESPACE_OPEN_SCOPE

struct hit_record;

struct scatter_record {
	Ray specularRay;
	bool isSpecular;
	Colour attenuation;
	PDF* PDF_ptr;
};

// Schlick approximation
inline double schlick(double cosine, double ref_idx) {
	double r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow(1 - cosine,5);
}

class Material {
public:
	virtual bool scatter(const Ray& r_in, const hit_record& rec, scatter_record& srec, MemoryArena& arena) const {
		return false;
	}

	virtual double bsdf(const Ray& r_in, const hit_record& rec, const Ray& r_out) const {
		return 0;
	}

	virtual Colour emitted(double u, double v, const Point3& p) const {
		return Colour(0, 0, 0);
	}
};

class Dialectric : public Material {
public: 
	double ref_idx;
	Dialectric(double ri) : ref_idx(ri) {}

	virtual bool scatter(const Ray& r_in, const hit_record& rec, scatter_record& srec, MemoryArena& arena) const override {
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

class DiffuseLight : public Material {
public:
	Colour albedo;
	DiffuseLight(const Colour& a) : albedo(a) {}

	virtual bool scatter(const Ray& r_in, const hit_record& rec, scatter_record& srec, MemoryArena& arena) const override {
		return false;
	}

	virtual Colour emitted(double u, double v, const Point3& p) const override {
		return albedo;
	}
};

class Lambertian : public Material {
public:
	std::shared_ptr<Texture> albedo;
	Lambertian(const Colour& a) : albedo(std::make_shared<SolidColour>(a)) {}
	Lambertian(std::shared_ptr<Texture> tex) : albedo(tex) {}

	virtual bool scatter(const Ray& r_in, const hit_record& rec, 
					     scatter_record& srec, MemoryArena& arena
						 ) const override {
		srec.isSpecular = false;
		srec.attenuation = albedo->value(rec.u, rec.v, rec);
		srec.PDF_ptr = ARENA_ALLOC(arena, CosinePDF)(rec.normal);
		return true;
	}

	virtual double bsdf(const Ray& r_in, const hit_record& rec, const Ray& r_out) const override {
		return 1 / pi;
	}
};

class Metal : public Material {
public:
	Colour albedo;
	double fuzz;
	Metal(const	Colour& a, double fuzz) : albedo(a), fuzz(fuzz < 1.0 ? fuzz : 1.0) {}

	// TODO: could probably rewrite this class to be a combination 
	// 	   of Lambertian and a mirror
	// This is a mixture between diffuse and specular
	virtual bool scatter(const Ray& r_in, const hit_record& rec, scatter_record& srec, MemoryArena& arena) const override {
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
			srec.PDF_ptr = ARENA_ALLOC(arena, CosinePDF)(rec.normal);
		}

		//scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		return true;
	}

	virtual double bsdf(const Ray& r_in, const hit_record& rec, const Ray& r_out) const override {
		return 1 / pi;
	}
};

RAY_NAMESPACE_CLOSE_SCOPE

#endif