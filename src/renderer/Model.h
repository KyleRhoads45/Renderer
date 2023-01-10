#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "ecs/Entity.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 textureCoordinate;
};

struct Mesh {
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;

	unsigned int numIndices;

	std::shared_ptr<Vertex[]> verts;
	std::shared_ptr<unsigned int[]> indices;
};

Entity InstantiateModel(const char* meshPath);