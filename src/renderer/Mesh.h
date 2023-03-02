#pragma once
#include <memory>
#include "Vertex.h"

struct Mesh {
	uint32_t vao;
	uint32_t vbo;
	uint32_t ebo;

	uint32_t numIndices;
	uint32_t numVerts;

	std::shared_ptr<Vertex[]> verts;
	std::shared_ptr<uint32_t[]> indices;

	void GenOpenGLBuffers();
};

