#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Mesh.h"

Mesh Mesh::FromFile(const char* meshPath) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	assert(scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode);

	return FromAssimpMesh(scene->mMeshes[0]);
}

Mesh Mesh::FromAssimpMesh(const aiMesh* meshData) {
	Mesh mesh;
	mesh.m_NumIndices = meshData->mNumFaces * 3;
	mesh.m_NumVerts = meshData->mNumVertices;

	mesh.m_Verts = MakeRef<Vertex[]>(meshData->mNumVertices);
	mesh.m_Indices = MakeRef<u32[]>(mesh.m_NumIndices);
	
	for (u32 i = 0; i < meshData->mNumVertices; i++) {
		Vertex vertex;

		const aiVector3D pos = meshData->mVertices[i];
		vertex.position = glm::vec3(pos.x, pos.y, pos.z);

		const aiVector3D normal = meshData->mNormals[i];
		vertex.normal = glm::vec3(normal.x, normal.y, normal.z);

		const aiVector3D tangent = meshData->mTangents[i];
		vertex.tangent = glm::vec3(tangent.x, tangent.y, tangent.z);

		if (meshData->HasTextureCoords(0)) {
			const aiVector3D textureCoord = meshData->mTextureCoords[0][i];
			vertex.textureCoord = glm::vec2(textureCoord.x, textureCoord.y);
		}

		mesh.m_Verts[i] = vertex;
	}

	u32 count = 0;
	for (u32 i = 0; i < meshData->mNumFaces; i++) {
		const aiFace face = meshData->mFaces[i];
		for (u32 j = 0; j < face.mNumIndices; j++) {
			mesh.m_Indices[count] = face.mIndices[j];
			count++;
		}
	}

	mesh.GenOpenGLBuffers();
	return mesh;
}

void Mesh::GenOpenGLBuffers() {
	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	glGenBuffers(1, &m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_NumVerts, &m_Verts[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoord));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glGenBuffers(1, &m_Ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * m_NumIndices, &m_Indices[0], GL_STATIC_DRAW);
}

void Mesh::UpdateVertexBuffer() const {
	glBindVertexArray(m_Vao);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_NumVerts, &m_Verts[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}
