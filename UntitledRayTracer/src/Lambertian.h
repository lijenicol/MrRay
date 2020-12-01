#pragma once
#include "rtutils.h"
#include "Material.h"
#include "Texture.h"

class Lambertian : public Material {
public:
	std::shared_ptr<Texture> albedo;
	Lambertian(const Colour& a) : albedo(std::make_shared<SolidColour>(a)) {}
	Lambertian(std::shared_ptr<Texture> tex) : albedo(tex) {}

	virtual bool scatter(const Ray& r_in, const hit_record& rec, Colour& attenuation, Ray& scattered) const override {
		Vec3 scattered_direction = rec.normal + random_unit_vector();
		scattered = Ray(rec.p, scattered_direction);
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}
};