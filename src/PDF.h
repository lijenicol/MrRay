#pragma once
#include "rtutils.h"
#include "ONB.h"


class PDF {
public:
	virtual ~PDF() {}
	virtual double value(const Vec3& direction) const = 0;
	virtual Vec3 generate() const = 0;
};


// Generates a random point given the cos(theta)/pi PDF
inline Vec3 randomCosineDirection() {
	double r1 = random_double();
	double r2 = random_double();
	double z = sqrt(1 - r2);

	double phi = 2 * pi * r1;
	double x = cos(phi) * sqrt(r2);
	double y = sin(phi) * sqrt(r2);

	return Vec3(x, y, z);
}

class CosinePDF : public PDF {
public:
	ONB uvw;
	CosinePDF(const Vec3& w) { uvw.buildFromW(w); }

	// cos/pi
	virtual double value(const Vec3& direction) const override {
		double cosine = dot(unit_vector(direction), uvw.w());
		return (cosine <= 0) ? 0 : cosine/pi;
	}

	virtual Vec3 generate() const override {
		return unit_vector(uvw.local(randomCosineDirection()));
	}
};


class HittablePDF : public PDF {
public:
	Point3 o;
	std::shared_ptr<Hittable> ptr;

	HittablePDF(std::shared_ptr<Hittable> p, const Point3& origin) : ptr(p), o(origin) {}

	virtual double value(const Vec3& direction) const override {
		return ptr->pdf_value(o, direction);
	}

	virtual Vec3 generate() const override {
		return ptr->random(o);
	}
};


class MixturePDF : public PDF {
public:
	std::shared_ptr<PDF> p[2];

	MixturePDF(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1) {
		p[0] = p0;
		p[1] = p1;
	}

	virtual double value(const Vec3& direction) const override {
		return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
	}

	virtual Vec3 generate() const override {
		if (random_double() < 0.5)
			return p[0]->generate();
		return p[1]->generate();
	}
};