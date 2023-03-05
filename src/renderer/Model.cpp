#include <iostream>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glad/glad.h>
#include "ecs/Registry.h"
#include "core/Components.h"
#include "Model.h"

Entity ProcessNode(const aiScene* scene, const aiNode* node, Transform* parent, Material* mat);
Mesh ProcessMesh(const aiMesh* mesh);

Entity InstantiateModel(const char* meshPath, Material* mat) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	assert(scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode);

	return ProcessNode(scene, scene->mRootNode, nullptr, mat);
}

Mesh LoadMesh(const char* meshPath) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	assert(scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode);

	return ProcessMesh(scene->mMeshes[0]);
}

Entity ProcessNode(const aiScene* scene, const aiNode* node, Transform* parent, Material* mat) {
	auto entity = Registry::Create();
	auto trans = Registry::AddComponent<Transform>(entity);

	aiVector3t<float> scale;
	aiVector3t<float> position;
	aiQuaterniont<float> rotation;
	node->mTransformation.Decompose(scale, rotation, position);
	
	trans->Set(
		glm::vec3(position.x, position.y, position.z), 
		glm::quat(rotation.w, rotation.x, rotation.y, rotation.z),
		glm::vec3(scale.x, scale.y, scale.z)
	);
	trans->entity = entity;

	if (parent != nullptr) {
		trans->SetParent(parent);
	}

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		auto meshRenderer = Registry::AddComponent<MeshRenderer>(entity);
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshRenderer->mesh = ProcessMesh(mesh);
		meshRenderer->material = mat;
	}

	uint32_t childCount = node->mNumChildren;
	std::shared_ptr<Transform* []> children = std::make_shared<Transform* []>(childCount);
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		auto child = ProcessNode(scene, node->mChildren[i], trans, mat);
		children[i] = child.GetComponent<Transform>();
	}

	trans->SetChildren(children, childCount);

	return entity;
}

Mesh ProcessMesh(const aiMesh* meshData) {
	Mesh mesh;
	mesh.numIndices = meshData->mNumFaces * 3;
	mesh.numVerts = meshData->mNumVertices;

	mesh.verts = std::make_shared<Vertex[]>(meshData->mNumVertices);
	mesh.indices = std::make_shared<unsigned int[]>(mesh.numIndices);
	
	for (unsigned int i = 0; i < meshData->mNumVertices; i++) {
		Vertex vertex;

		const aiVector3D pos = meshData->mVertices[i];
		vertex.position = glm::vec3(pos.x, pos.y, pos.z);

		const aiVector3D normal = meshData->mNormals[i];
		vertex.normal = glm::vec3(normal.x, normal.y, normal.z);

		if (meshData->HasTextureCoords(0)) {
			const aiVector3D textureCoord = meshData->mTextureCoords[0][i];
			vertex.textureCoord = glm::vec2(textureCoord.x, textureCoord.y);
		}

		mesh.verts[i] = vertex;
	}

	int count = 0;
	for (uint32_t i = 0; i < meshData->mNumFaces; i++) {
		const aiFace face = meshData->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++) {
			mesh.indices[count] = face.mIndices[j];
			count++;
		}
	}

	mesh.GenOpenGLBuffers();
	return mesh;
}
