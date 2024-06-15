#pragma once
#include <assimp/scene.h>
#include "core/Base.h"
#include "Vertex.h"

class Mesh {
public:
	static Mesh FromAssimpMesh(const aiMesh* meshData);
	static Mesh FromFile(const char* meshPath);

	void GenOpenGLBuffers();
	void UpdateVertexBuffer() const;
public:
	u32 m_Vao;
	u32 m_Vbo;
	u32 m_Ebo;

	u32 m_NumIndices;
	u32 m_NumVerts;

	Ref<Vertex[]> m_Verts;
	Ref<u32[]> m_Indices;
};

