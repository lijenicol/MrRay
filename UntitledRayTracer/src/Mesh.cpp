#include "Mesh.h"
#include "HittableList.h"
#include "Triangle.h"
#include <sstream>
#include <fstream>

// Setup triangles and BVH structure
Mesh::Mesh(std::string filename, const Vec3 pos, std::shared_ptr<Material> mat) : pos(pos), mat(mat) {
	std::cerr << "Loading Mesh: " << filename << "\n";
	HittableList triangles;

	std::vector<Vec3> triangleIndices;
	std::vector<Vec3> normalIndices;
	std::vector<Vec3> texCoordIndices;

	std::vector<Vec3> inputNormals;
	std::vector<Vec3> inputPoints;
	std::vector<Vec3> inputTexCoords;
	std::ifstream objStream(filename);
	std::string line;
	if (objStream.is_open()) {
		//go through every line of the obj file
		while (getline(objStream, line)) {
			std::stringstream ss;
			ss << line;

			//check for vertex label
			std::string label;
			ss >> label;
			if (label == "v") {
				//construct a point from the line
				Vec3 point;
				ss >> point.e[0];
				ss >> point.e[1];
				ss >> point.e[2];

				//push the point to the point list
				inputPoints.push_back(point);
			}
			if (label == "vn") {
				Vec3 normal;
				ss >> normal.e[0];
				ss >> normal.e[1];
				ss >> normal.e[2];

				//push the normal to the normals list
				inputNormals.push_back(normal);
			}
			if (label == "vt") {
				Vec3 texCoord;
				ss >> texCoord.e[0];
				ss >> texCoord.e[1];

				//push the coord to the texcoords list
				inputTexCoords.push_back(texCoord);
			}
			if (label == "f") {
				Vec3 triangle;
				Vec3 normalIndex;
				Vec3 texCoordIndex;
				std::string s;
				int pos;
				std::string v;
				std::string u;

				//search face for / delimeter
				ss >> s;
				pos = s.find("/");
				v = s.substr(0, pos);
				triangle.e[0] = std::stoi(v) - 1;
				s.erase(0, pos + 1);
				pos = s.find("/");
				u = s.substr(0, pos);
				if (u.length() != 0) {
					texCoordIndex.e[0] = std::stoi(u) - 1;
				}
				s.erase(0, pos + 1);
				normalIndex.e[0] = std::stoi(s) - 1;

				ss >> s;
				pos = s.find("/");
				v = s.substr(0, pos);
				triangle.e[1] = std::stoi(v) - 1;
				s.erase(0, pos + 1);
				pos = s.find("/");
				u = s.substr(0, pos);
				if (u.length() != 0) {
					texCoordIndex.e[1] = std::stoi(u) - 1;
				}
				s.erase(0, pos + 1);
				normalIndex.e[1] = std::stoi(s) - 1;

				ss >> s;
				pos = s.find("/");
				v = s.substr(0, pos);
				triangle.e[2] = std::stoi(v) - 1;
				s.erase(0, pos + 1);
				pos = s.find("/");
				u = s.substr(0, pos);
				if (u.length() != 0) {
					texCoordIndex.e[2] = std::stoi(u) - 1;
				}
				s.erase(0, pos + 1);
				normalIndex.e[2] = std::stoi(s) - 1;

				triangleIndices.push_back(triangle);
				normalIndices.push_back(normalIndex);
				texCoordIndices.push_back(texCoordIndex);
			}
		}
		objStream.close();
	}
	else {
		std::cout << "File cannot be read or does not exist: " << filename;
	}

	// Construct triangle instances from indices
	for (unsigned int i = 0; i < triangleIndices.size(); i++) {
		auto v0 = std::make_shared<vertex_triangle>();
		auto v1 = std::make_shared<vertex_triangle>();
		auto v2 = std::make_shared<vertex_triangle>();

		v0->pos = inputPoints[triangleIndices[i].x()];
		v1->pos = inputPoints[triangleIndices[i].y()];
		v2->pos = inputPoints[triangleIndices[i].z()];

		if (inputNormals.size() != 0) {
			v0->normal = inputNormals[normalIndices[i].x()];
			v1->normal = inputNormals[normalIndices[i].y()];
			v2->normal = inputNormals[normalIndices[i].z()];
		}

		if (inputTexCoords.size() != 0) {
			v0->u = inputTexCoords[texCoordIndices[i].x()].x();
			v0->v = inputTexCoords[texCoordIndices[i].x()].y();
			v1->u = inputTexCoords[texCoordIndices[i].y()].x();
			v1->v = inputTexCoords[texCoordIndices[i].y()].y();
			v2->u = inputTexCoords[texCoordIndices[i].z()].x();
			v2->v = inputTexCoords[texCoordIndices[i].z()].y();
		}

		triangles.add(std::make_shared<Triangle>(v0,v1,v2, mat));
	}

	// This will need to change if animation is added
	container = new BVHNode(triangles, 0, 0);
	std::cerr << "Mesh Loaded Successfully\n\n";
}

// Test hit by intersecting the mesh container
bool Mesh::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
	return container->hit(r, t_min, t_max, rec);
}

// Get bounding box from the mesh container BVH
bool Mesh::bounding_box(double time0, double time1, AABB& output_box) const {
	return container->bounding_box(time0, time1, output_box);
}