#include "Base.h"
#include "Primatives.h"

Mesh Primatives::Cube(bool invert = false) {
	const u32 vertCount = 8;
	glm::vec3 verts[vertCount] {

		// Front face
		glm::vec3(-1, -1, 1),
		glm::vec3(1, -1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(-1, 1, 1),

		// Right face
		glm::vec3(1, -1, -1),
		glm::vec3(1, 1, -1),

		// Left face
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, 1, -1),

	};

	const u32 indexCount = 36;
	u32 indices[indexCount] {

		// Front face
		0, 1, 2,
		0, 2, 3,

		// Right face
		1, 4, 5,
		1, 5, 2,

		// Left face
		6, 0, 3,
		6, 3, 7,

		// Back face
		4, 6, 7,
		4, 7, 5,

		// Top face
		3, 2, 5,
		3, 5, 7,

		// Bottom face
		6, 4, 1,
		6, 1, 0,
	};

	if (invert) {
		for (i32 i = 0; i < indexCount; i += 3) {
			i32 first = indices[i];
			i32 third = indices[i + 2];

			indices[i] = third;
			indices[i + 2] = first;
		}
	}

	Mesh cube;

	cube.m_NumVerts = vertCount;
	cube.m_Verts = MakeRef<Vertex[]>(vertCount);

	for (i32 i = 0; i < vertCount; i++) {
		Vertex vert;
		vert.position = verts[i];
		cube.m_Verts[i] = vert;
	}

	cube.m_NumIndices = indexCount;
	cube.m_Indices = MakeRef<u32[]>(indexCount);

	for (i32 i = 0; i < indexCount; i++) {
		cube.m_Indices[i] = indices[i];
	}
	
	cube.GenOpenGLBuffers();
	return cube;
}

Mesh Primatives::Plane() {
	const size_t vertCount = 4;
	glm::vec3 verts[vertCount] {
		glm::vec3(.5, -1, 0),
		glm::vec3(1, -1, 0),
		glm::vec3(1, -.5, 0),
		glm::vec3(.5, -.5, 0),
	};

	glm::vec2 texCoords[vertCount] {
		glm::vec2(0.0, 0.0),
		glm::vec2(1.0, 0.0),
		glm::vec2(1.0, 1.0),
		glm::vec2(0.0, 1.0)
	};

	const u32 indexCount = 6;
	u32 indices[indexCount] {
		0, 1, 2,
		0, 2, 3,
	};

	Mesh plane;

	plane.m_NumVerts = vertCount;
	plane.m_Verts = MakeRef<Vertex[]>(vertCount);

	for (i32 i = 0; i < vertCount; i++) {
		Vertex vert;
		vert.position = verts[i];
		vert.normal = glm::vec3(0, 0, -1);
		vert.tangent = glm::vec3(0, 0, 1);
		vert.textureCoord = texCoords[i];

		plane.m_Verts[i] = vert;
	}

	plane.m_NumIndices = indexCount;
	plane.m_Indices = MakeRef<u32[]>(indexCount);

	for (i32 i = 0; i < indexCount; i++) {
		plane.m_Indices[i] = indices[i];
	}
	
	plane.GenOpenGLBuffers();
	return plane;
}
