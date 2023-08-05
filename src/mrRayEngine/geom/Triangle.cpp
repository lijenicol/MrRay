#include "mrRay/geom/Triangle.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

Triangle::Triangle(
    int* positionIndex, int* normalIndex, int* uvIndex,
    Mesh* parentMesh, std::shared_ptr<Material> mat)
    : _positionIndex(positionIndex), _normalIndex(normalIndex),
      _uvIndex(uvIndex), _parentMesh(parentMesh), _mat(mat)
{
    auto vertexPositions = getVertexPositions();
    Vec3 v0 = std::get<0>(vertexPositions);
    Vec3 v1 = std::get<1>(vertexPositions);
    Vec3 v2 = std::get<2>(vertexPositions);

    _normal = unit_vector(cross(v1 - v0, v2 - v0));

    // Pre-calculate bounding box
    Point3 a(
            fmin(fmin(v0.e[0], v1.e[0]), v2.e[0]),
            fmin(fmin(v0.e[1], v1.e[1]), v2.e[1]),
            fmin(fmin(v0.e[2], v1.e[2]), v2.e[2])
    );
    Point3 b(
            fmax(fmax(v0.e[0], v1.e[0]), v2.e[0]),
            fmax(fmax(v0.e[1], v1.e[1]), v2.e[1]),
            fmax(fmax(v0.e[2], v1.e[2]), v2.e[2])
    );
    _boundingBox = AABB(a, b);
}

bool
Triangle::hit(
    const Ray& r, double t_min, double t_max, hit_record& rec) const
{
	// Return if ray is parallel with triangle
	if (dot(_normal, r.direction()) == 0) {
		return false;
	}

    auto vertexPositions = getVertexPositions();
    Vec3 v0 = std::get<0>(vertexPositions);
    Vec3 v1 = std::get<1>(vertexPositions);
    Vec3 v2 = std::get<2>(vertexPositions);

	// Calculate plane intersection
	float d = dot(_normal, v0);
	float t = (d - dot(_normal, r.origin())) / dot(_normal, r.direction());
	Point3 intersectionPoint = r.origin() + t * r.direction();

	// Don't report hits outside the range
	if (t > t_max || t < t_min) {
        return false;
    }

	// Calculate if intersection point is within triangle
	float testOne = dot(cross(v1 - v0, intersectionPoint - v0), _normal);
	if (testOne < 0) {
		return false;
	}

	float testTwo = dot(cross(v2 - v1, intersectionPoint - v1), _normal);
	if (testTwo < 0) {
		return false;
	}

	float testThree = dot(cross(v0 - v2, intersectionPoint - v2), _normal);
	if (testThree < 0) {
		return false;
	}

	rec.t = t;
    rec.p = intersectionPoint;
    rec.mat = _parentMesh->mat.get();

    Vec3 barycentric = getTriangleBarycentric(intersectionPoint);
    if (_uvIndex != nullptr) {
        getInterpolatedUV(barycentric, rec.u, rec.v);
    }
    else {
        rec.u = 0;
        rec.v = 0;
    }

	if (_normalIndex != nullptr && _parentMesh->smoothShading) {
		Vec3 interpolatedNormal = getInterpolatedNormal(barycentric);
		rec.set_face_normal(r, interpolatedNormal);
	}
	else {
		rec.set_face_normal(r, _normal);
	}
	
	return true;
}

std::tuple<Vec3, Vec3, Vec3>
Triangle::getVertexPositions() const
{
    return {
        _parentMesh->positions[*_positionIndex],
        _parentMesh->positions[*(_positionIndex + 1)],
        _parentMesh->positions[*(_positionIndex + 2)],
    };
}

Vec3
Triangle::getTriangleBarycentric(const Vec3& p) const
{
    auto vertexPositions = getVertexPositions();
    Vec3 v0 = std::get<0>(vertexPositions);
    Vec3 v1 = std::get<1>(vertexPositions);
    Vec3 v2 = std::get<2>(vertexPositions);

	// Get triangle areas
	double total_area_2 = cross(v1 - v0, v2 - v0).length();
	double a_area_2 = cross(p - v2, p - v1).length();
	double b_area_2 = cross(p - v0, p - v2).length();
	double c_area_2 = cross(p - v1, p - v0).length();

	// Compute barycentric
	double wa = a_area_2 / total_area_2;
	double wb = b_area_2 / total_area_2;
	double wc = c_area_2 / total_area_2;

	return Vec3(wa, wb, wc);
}

void
Triangle::getInterpolatedUV(const Vec3& w, double& u, double& v) const
{
    Vec3 uv0 = _parentMesh->uvs[*_uvIndex];
    Vec3 uv1 = _parentMesh->uvs[*(_uvIndex + 1)];
    Vec3 uv2 = _parentMesh->uvs[*(_uvIndex + 2)];

	u = uv0[0] * w[0] + uv1[0] * w[1] + uv2[0] * w[2];
	v = uv0[1] * w[0] + uv1[1] * w[1] + uv2[1] * w[2];
}

Vec3
Triangle::getInterpolatedNormal(const Vec3& w) const
{
    Vec3 n0 = _parentMesh->normals[*_normalIndex];
    Vec3 n1 = _parentMesh->normals[*(_normalIndex + 1)];
    Vec3 n2 = _parentMesh->normals[*(_normalIndex + 2)];

	// Compute interpolated normal
	double x = n0[0] * w[0] + n1[0] * w[1] + n2[0] * w[2];
	double y = n0[1] * w[0] + n1[1] * w[1] + n2[1] * w[2];
	double z = n0[2] * w[0] + n1[2] * w[1] + n2[2] * w[2];
	return unit_vector(Vec3(x,y,z));
}

bool
Triangle::bounding_box(double time0, double time1, AABB& output_box) const
{
	output_box = _boundingBox;
	return true;
}

MR_RAY_NAMESPACE_CLOSE_SCOPE
