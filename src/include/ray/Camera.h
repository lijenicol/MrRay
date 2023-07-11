#ifndef RAY_CAMERA_H
#define RAY_CAMERA_H

#include "ray/namespace.h"
#include "ray/rtutils.h"
#include "ray/sampler.h"

RAY_NAMESPACE_OPEN_SCOPE

class Camera {
	public: 
		Camera(const Point3& lookFrom, const Point3& lookAt, const Vec3& vup,
               double vfov, double aspect_ratio, double aperture,
               double focus_dist)
        {
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
			lowerLeftCorner
                = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

			lensRadius = aperture / 2;
		}

		Ray getRay(const double& s, const double& t, Sampler& sampler) const
        {
			Vec3 rd = lensRadius * sampler.inUnitDisk();
			Vec3 offset = u * rd.x() + v * rd.y();
			return Ray(
                origin + offset,
                unit_vector(
                    lowerLeftCorner + s * horizontal
                    + t * vertical - origin - offset));
		}

	private:
		Point3 origin;
		Vec3 horizontal;
		Vec3 vertical;
		Point3 lowerLeftCorner;
		Vec3 u, v, w;
		double lensRadius;
};

RAY_NAMESPACE_CLOSE_SCOPE

#endif
