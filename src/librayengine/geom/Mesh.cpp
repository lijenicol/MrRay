#include "ray/geom/Mesh.h"
#include "ray/geom/HittableList.h"
#include <sstream>
#include <fstream>

RAY_NAMESPACE_OPEN_SCOPE

// Split a string into multiple pieces at delimeter points
std::vector<std::string> splitString(std::string s, char delimeter) {
	std::vector<std::string> splits;

	// Find occurence of white space and split
	size_t pos;
	while ((pos = s.find(delimeter)) != std::string::npos) {
		// Push empty string if delim occurs right away
		if (pos == 0) {
			splits.push_back("");
		}
		
		std::string token = s.substr(0, pos);
		s.erase(0, pos + 1);
		splits.push_back(token);
	}

	// Add the last part of the string
	splits.push_back(s);
	return splits;
}

// Setup triangles and BVH structure
Mesh::Mesh(std::string filename, double scale, const bool smooth_shading, std::shared_ptr<Material> mat) 
	: smooth_shading(smooth_shading), mat(mat), scale(scale) {
	std::cerr << "Loading Mesh: " << filename << "\n";
	HittableList trianglesHL;

	std::vector<Vec3> triangleIndices;
	std::vector<Vec3> texCoordIndices;
	std::vector<Vec3> normalIndices;

	std::vector<Vec3> inputPoints;
	std::vector<Vec3> inputTexCoords;
	std::vector<Vec3> inputNormals;

	std::ifstream objStream(filename);
	std::string line;
	if (objStream.is_open()) {
		while (getline(objStream, line)) {
			// Split string
			auto parts = splitString(line, ' ');

			// Parse vertices
			if (parts[0] == "v") {
				//construct a point from the line
				Vec3 point;
				point.e[0] = std::stod(parts[1]);
				point.e[1] = std::stod(parts[2]);
				point.e[2] = std::stod(parts[3]);

				//push the point to the point list
				inputPoints.push_back(point);
			}

			// Parse normals
			if (parts[0] == "vn") {
				Vec3 normal;
				normal.e[0] = std::stod(parts[1]);
				normal.e[1] = std::stod(parts[2]);
				normal.e[2] = std::stod(parts[3]);

				//push the normal to the normals list
				inputNormals.push_back(normal);
			}

			// Parse texture coords
			if (parts[0] == "vt") {
				Vec3 texCoord;
				texCoord.e[0] = std::stod(parts[1]);
				texCoord.e[1] = std::stod(parts[2]);

				//push the coord to the texcoords list
				inputTexCoords.push_back(texCoord);
			}

			// Parse faces
			if (parts[0] == "f") {
				// Faces are made up of a vertex index, texture coord
				// index, and also a normal index.
				Vec3 triangle;
				Vec3 normalIndex;
				Vec3 texCoordIndex;

				// Extract all index info from the line
				for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
					auto vertexInfo = splitString(parts[vertexIndex + 1], '/');
					triangle.e[vertexIndex] = std::stod(vertexInfo[0]) - 1;
					if (vertexInfo[1] != "") {
						texCoordIndex.e[vertexIndex] = std::stod(vertexInfo[1]) - 1;
					}
					normalIndex.e[vertexIndex] = std::stod(vertexInfo[2]) - 1;
				}

				triangleIndices.push_back(triangle);
				normalIndices.push_back(normalIndex);
				texCoordIndices.push_back(texCoordIndex);
			}
		}
		objStream.close();
	}
	else {
		std::cerr << "File cannot be read or does not exist: " << filename << "\n";
		exit(EXIT_FAILURE);
	}

	// Construct triangle instances from indices and scale vertices
	std::cerr << "Reordering Vertices and Creating Triangles\n";
	for (unsigned int i = 0; i < triangleIndices.size(); i++) {
		auto v0 = std::make_shared<vertex_triangle>();
		auto v1 = std::make_shared<vertex_triangle>();
		auto v2 = std::make_shared<vertex_triangle>();

		v0->pos = inputPoints[triangleIndices[i].x()] * scale;
		v1->pos = inputPoints[triangleIndices[i].y()] * scale;
		v2->pos = inputPoints[triangleIndices[i].z()] * scale;

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

		// Construct a triangle. If normals aren't specified, never use 
		// smooth shading (this is a bit of a hack)
		std::shared_ptr<Triangle> tri = std::make_shared<Triangle>(v0, v1, v2, mat,
			inputNormals.size() != 0 ? smooth_shading : false);
		trianglesHL.add(tri);
		triangles.push_back(tri);
	}

	// Set metadata
	vertexCount = inputPoints.size();
	faceCount = triangleIndices.size();

	// This will need to change if animation is added
	std::cerr << "Creating BVH.\n";
	container = new BVHNode(trianglesHL, 0, 0);
	std::cerr << "Mesh Loaded Successfully\n\n";
}

void Mesh::print(bool verbose) {
	std::cerr << "NUMBER OF VERTICES: " << vertexCount << std::endl;
	std::cerr << "NUMBER OF FACES: " << faceCount << "\n\n";

	if (verbose) {
		for (int i = 0; i < faceCount; i++) {
			// Print triangle number
			std::cerr << "TRIANGLE ID: " << i << std::endl;
			std::cerr << "NORMAL: " << triangles[i].get()->normal << "\n\n";
		}
	}
}

void Mesh::printHitTriangles() {
	std::vector<int> hitList;
	std::vector<int> noHitList;
	std::vector<int> testedList;
	std::vector<int> notTestedList;

	for (int i = 0; i < triangles.size(); i++) {
		// Put IDs in hit list
		if (*(triangles[i].get()->hasBeenHit))
			hitList.push_back(i);
		else
			noHitList.push_back(i);

		// Put IDs in tested list
		if (*(triangles[i].get()->hasBeenTested))
			testedList.push_back(i);
		else
			notTestedList.push_back(i);
	}

	// Print tested list
	if (testedList.size() > 0) {
		std::cerr << "TESTED LIST" << std::endl;
		std::cerr << testedList[0];
		for (int i = 1; i < testedList.size(); i++) {
			std::cerr << ", " << testedList[i];
		}
		std::cerr << "\n\n";
	}

	// Print not tested list
	if (notTestedList.size() > 0) {
		std::cerr << "NOT TESTED LIST" << std::endl;
		std::cerr << notTestedList[0];
		for (int i = 1; i < notTestedList.size(); i++) {
			std::cerr << ", " << notTestedList[i];
		}
		std::cerr << "\n\n";
	}

	// Print hit list
	if (hitList.size() > 0) {
		std::cerr << "HIT LIST" << std::endl;
		std::cerr << hitList[0];
		for (int i = 1; i < hitList.size(); i++) {
			std::cerr << ", " << hitList[i];
		}
		std::cerr << "\n\n";
	}

	// Print no hit list
	if (noHitList.size() > 0) {
		std::cerr << "NO HIT LIST" << std::endl;
		std::cerr << noHitList[0];
		for (int i = 1; i < noHitList.size(); i++) {
			std::cerr << ", " << noHitList[i];
		}
		std::cerr << std::endl;
	}
}

// Test hit by intersecting the mesh container
bool Mesh::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
	return container->hit(r, t_min, t_max, rec);
}

// Get bounding box from the mesh container BVH
bool Mesh::bounding_box(double time0, double time1, AABB& output_box) const {
	return container->bounding_box(time0, time1, output_box);
}

RAY_NAMESPACE_CLOSE_SCOPE
