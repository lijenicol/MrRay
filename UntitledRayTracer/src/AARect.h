#pragma once
#include "rtutils.h"
#include "Hittable.h"

class XYRect : public Hittable {
	public:
		std::shared_ptr<Material> mat;
		double x0, x1, y0, y1, k;

		XYRect(double x0, double x1, double y0, double y1, double k, std::shared_ptr<Material> mat) 
			: x0(x0), x1(x1), y0(y0), y1(y1), k(k), mat(mat) {}

		virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
};

class XZRect : public Hittable {
	public:
		std::shared_ptr<Material> mat;
		double x0, x1, z0, z1, k;

		XZRect(double x0, double x1, double y0, double y1, double k, std::shared_ptr<Material> mat)
			: x0(x0), x1(x1), z0(y0), z1(y1), k(k), mat(mat) {}

		virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
};

class YZRect : public Hittable {
	public:
		std::shared_ptr<Material> mat;
		double y0, y1, z0, z1, k;

		YZRect(double x0, double x1, double y0, double y1, double k, std::shared_ptr<Material> mat)
			: y0(x0), y1(x1), z0(y0), z1(y1), k(k), mat(mat) {}

		virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
};