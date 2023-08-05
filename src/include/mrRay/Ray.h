#ifndef MR_RAY_H
#define MR_RAY_H

#include "mrRay/namespace.h"
#include "mrRay/Vec3.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

class Ray {
	public: 
		Point3 orig;
		Vec3 dir;

		Ray() {}
		Ray(const Point3& origin, const Vec3& direction) 
			: orig(origin), dir(direction) 
		{}

		Point3 origin() const { return orig; }
		Vec3 direction() const { return dir; }

		Point3 at(double t) const { return orig + t * dir; }
};

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif