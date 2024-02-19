#include <filesystem>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "core/Base.h"
#include "Model.h"
#include <iostream>

Entity Model::Instantiate(const char* meshPath, Material* mat) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	assert(scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode);

	std::string directoryPath(meshPath);
	directoryPath = directoryPath.substr(0, directoryPath.find_last_of("\\/"));

	return ProcessNode(directoryPath, scene, scene->mRootNode, Entity::Null(), mat);
}

Entity Model::ProcessNode(const std::string& directoryPath, const aiScene* scene, const aiNode* node, Entity parent, Material* mat) {
	auto entity = Registry::Create();
	entity.Add<LocalToWorld>();
	auto& trans = entity.Add<Transform>();

	aiVector3t<f32> scale;
	aiVector3t<f32> position;
	aiQuaterniont<f32> rotation;
	node->mTransformation.Decompose(scale, rotation, position);
	
	trans.position = glm::vec3(position.x, position.y, position.z);
	trans.rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
	trans.scale = glm::vec3(scale.x, scale.y, scale.z);

	if (parent != Entity::Null()) {
		entity.Add<Parent>().entity = parent;
	}

	for (u32 i = 0; i < node->mNumMeshes; i++) {
		auto& meshRenderer = entity.Add<MeshRenderer>();
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		const aiMaterial* meshMaterial = scene->mMaterials[mesh->mMaterialIndex];
		u32 diffuseCount = meshMaterial->GetTextureCount(aiTextureType_DIFFUSE);

		if (diffuseCount > 0) {
			aiString textureName;
			meshMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &textureName);

			std::string texturePath = textureName.C_Str();

			ASSERT(texturePath.find("\\/") == std::string::npos, "Textures need to be in same directory as model");

			texturePath = directoryPath + "/" + texturePath;

			Ref<Texture> diffuse = Texture::Load(texturePath);
			Material* betterMat = Material::NewStarndardMaterial();
			betterMat->SetDiffuse(diffuse);

			meshRenderer.mesh = Mesh::FromAssimpMesh(mesh);
			meshRenderer.material = betterMat;
		}
		else {
			Ref<Texture> diffuse = Texture::Load("res/Missing.png");
			Material* betterMat = Material::NewStarndardMaterial();
			betterMat->SetDiffuse(diffuse);

			meshRenderer.mesh = Mesh::FromAssimpMesh(mesh);
			meshRenderer.material = betterMat;
		}
	}

	if (node->mNumChildren > 0) {
		entity.Add<Children>();
		for (u32 i = 0; i < node->mNumChildren; i++) {
			auto child = ProcessNode(directoryPath, scene, node->mChildren[i], entity, mat);
			entity.Get<Children>().entities.push_back(child);
		}
	}

	return entity;
}
