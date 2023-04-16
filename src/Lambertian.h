#pragma once
#include "rtutils.h"
#include "Material.h"
#include "Texture.h"
#include "ONB.h"

class Lambertian : public Material {
public:
	std::shared_ptr<Texture> albedo;
	Lambertian(const Colour& a) : albedo(std::make_shared<SolidColour>(a)) {}
	Lambertian(std::shared_ptr<Texture> tex) : albedo(tex) {}

	virtual bool scatter(const Ray& r_in, const hit_record& rec, scatter_record& srec) const override {
		srec.isSpecular = false;
		srec.attenuation = albedo->value(rec.u, rec.v, rec);
		srec.PDF_ptr = std::make_shared<CosinePDF>(rec.normal);
		return true;
	}

	virtual double bsdf(const Ray& r_in, const hit_record& rec, const Ray& r_out) const override {
		return 1 / pi;
	}
};