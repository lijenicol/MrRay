#pragma once
#include "rtutils.h"
#include "sampler.h"

class Camera {
	public: 
		Camera(Point3 lookFrom, Point3 lookAt, Vec3 vup, double vfov, double aspect_ratio, double aperture, double focus_dist) {
			double theta = degrees_to_radians(vfov);
			double h = tan(theta / 2);
			double viewport_height = 2.0 * h;
			double viewport_width = aspect_ratio * viewport_height;

			w = unit_vector(lookFrom - lookAt);
			u = unit_vector(cross(vup,w));
			v = unit_vector(cross(w, u));

			origin = lookFrom;
			horizontal = focus_dist * viewport_width * u;
			vertical = focus_dist * viewport_height * v;
			lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

			lens_radius = aperture / 2;
		}

		Ray get_ray(double s, double t, Sampler& sampler) const {
			Vec3 rd = lens_radius * sampler.inUnitDisk();
			Vec3 offset = u * rd.x() + v * rd.y();
			return Ray(origin + offset, unit_vector(lower_left_corner + s * horizontal + t * vertical - origin - offset));
		}

	private:
		Point3 origin;
		Vec3 horizontal;
		Vec3 vertical;
		Point3 lower_left_corner;
		Vec3 u, v, w;
		double lens_radius;
};