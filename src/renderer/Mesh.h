#pragma once
#include <memory>
#include <assimp/scene.h>
#include "Vertex.h"

class Mesh {
public:
	static Mesh FromAssimpMesh(const aiMesh* meshData);
	static Mesh FromFile(const char* meshPath);

	void GenOpenGLBuffers();
	void UpdateVertexBuffer();

	uint32_t vao;
	uint32_t vbo;
	uint32_t ebo;

	uint32_t numIndices;
	uint32_t numVerts;

	std::shared_ptr<Vertex[]> verts;
	std::shared_ptr<uint32_t[]> indices;
};

