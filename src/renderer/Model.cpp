#include <iostream>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "core/Base.h"
#include "ecs/Registry.h"
#include "Model.h"

Entity Model::Instantiate(const char* meshPath, Material* mat) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	assert(scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode);

	return ProcessNode(scene, scene->mRootNode, nullptr, mat);
}

Entity Model::ProcessNode(const aiScene* scene, const aiNode* node, Transform* parent, Material* mat) {
	const auto entity = Registry::Create();
	auto& trans = Registry::Add<Transform>(entity);

	aiVector3t<f32> scale;
	aiVector3t<f32> position;
	aiQuaterniont<f32> rotation;
	node->mTransformation.Decompose(scale, rotation, position);
	
	trans.position = glm::vec3(position.x, position.y, position.z);
	trans.rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
	trans.scale = glm::vec3(scale.x, scale.y, scale.z);

	if (parent != nullptr) {
		//trans.SetParent(parent);
	}

	for (u32 i = 0; i < node->mNumMeshes; i++) {
		auto& meshRenderer = Registry::Add<MeshRenderer>(entity);
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshRenderer.mesh = Mesh::FromAssimpMesh(mesh);
		meshRenderer.material = mat;
	}

	for (u32 i = 0; i < node->mNumChildren; i++) {
		auto child = ProcessNode(scene, node->mChildren[i], &trans, mat);
		//trans.AddChild(&Registry::Get<Transform>(child));
	}

	return entity;
}

