#pragma once
#include <assimp/scene.h>
#include "core/Base.h"
#include "Vertex.h"

class Mesh {
public:
	static Mesh FromAssimpMesh(const aiMesh* meshData);
	static Mesh FromFile(const char* meshPath);

	void GenOpenGLBuffers();
	void UpdateVertexBuffer();

	u32 vao;
	u32 vbo;
	u32 ebo;

	u32 numIndices;
	u32 numVerts;

	Ref<Vertex[]> verts;
	Ref<u32[]> indices;
};

