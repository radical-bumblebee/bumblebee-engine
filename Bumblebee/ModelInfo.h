#ifndef _ModelInfo_H_
#define _ModelInfo_H_

#include "Dependencies/include/glm/vec4.hpp"
#include "Dependencies/include/glm/vec3.hpp"

#include <memory>
#include <vector>
#include <string>

#define PROPERTIES 6
#define FACES 3

// Contains position and normal coordinates
class PlyVertex {
public:
	PlyVertex() : x(0.0f), y(0.0f), z(0.0f), nx(0.0f), ny(0.0f), nz(0.0f), uv_x(0.0f), uv_y(0.0f) { };
	PlyVertex(float ix, float iy, float iz, float inx = 0.0f, float iny = 1.0f, float inz = 0.0f, float inuv_x = 0.0f, float inuv_y = 0.0f) :
		x(ix), y(iy), z(iz),
		nx(inx), ny(iny), nz(inz),
		uv_x(inuv_x), uv_y(inuv_y) { };
	~PlyVertex() { };

	float x;
	float y;
	float z;
	float nx;
	float ny;
	float nz;
	float uv_x;
	float uv_y;
};

// Contains triangle vertices
class PlyFace {
public:
	PlyFace() { };
	PlyFace(int ia, int ib, int ic) :
		a(ia), b(ib), c(ic) { };
	~PlyFace() { };

	int a;
	int b;
	int c;
};

// Contains model data
class ModelInfo {
public:
	typedef std::shared_ptr<ModelInfo> ptr;

	ModelInfo() : num_vertices(0), num_faces(0), vao_id(0), vbo_id(0), ib_id(0), shininess(0.0f), transparency(1.0f), update_mesh(false), visible(true) { };
	~ModelInfo() { };

	std::string name;
	int num_vertices;
	int num_faces;

	std::vector<PlyFace> faces;
	std::vector<PlyVertex> vertices;

	unsigned int vbo_id;
	unsigned int vao_id;
	unsigned int ib_id;

	glm::vec4 color;
	glm::vec3 scale;
	float shininess;
	float transparency;
	bool update_mesh;
	bool visible;
};

#endif