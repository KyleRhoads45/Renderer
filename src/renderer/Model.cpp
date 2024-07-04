#include <filesystem>
#include <iostream>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "core/Base.h"
#include "core/Serializer.h"
#include "Model.h"

Entity Model::Instantiate(const char* importedModelFile) {
	std::vector<YAML::Node> nodes = YAML::LoadAllFromFile(importedModelFile);

	YAML::Node& header = nodes.front();
	const auto& modelFile = header["SourceFile"].as<std::string>();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelFile, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	assert(scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode);

	Entity rootEntity = Entity::Null();

	std::vector<Entity> entityLookUp;

	for (size_t i = 1; i < nodes.size(); i++) {
		YAML::Node node = nodes[i];

		Entity entity = Registry::Create();
		entityLookUp.push_back(entity);
		entity.Add<LocalToWorld>();

		YAML::Node components = node["Components"];
		YAML::Node transNode = components["Transform"];

		const auto& pos   = transNode["Position"].as<glm::vec3>();
		const auto& rot   = transNode["Orientation"].as<glm::quat>();
		const auto& scale = transNode["Scale"].as<glm::vec3>();

		auto& trans = entity.Add<Transform>();
		trans.position = pos;
		trans.rotation = rot;
		trans.scale    = scale;

		if (YAML::Node parentNode = node["ParentId"]; !parentNode.IsNull()) {
			i32 parentIndex = parentNode.as<i32>();
			Entity parentEntity = entityLookUp[parentIndex];

			entity.Add<Parent>().entity = parentEntity;
			parentEntity.GetAdd<Children>().entities.push_back(entity);
		}

		YAML::Node meshRendrNode = components["MeshRenderer"];
		YAML::Node meshIndicesNode = meshRendrNode["MeshIndicies"];
		YAML::Node materialsNode = meshRendrNode["Materials"];

		if (meshIndicesNode.IsNull() || materialsNode.IsNull()) continue;

		auto& meshRenderer = entity.Add<MeshRenderer>();

		const auto& meshIndicies = meshIndicesNode.as<std::vector<i32>>();
		for (const i32 meshIndex : meshIndicies) {
			meshRenderer.meshes.push_back(Mesh::FromAssimpMesh(scene->mMeshes[meshIndex]));
		}

		const auto& materialFiles = materialsNode.as<std::vector<std::string>>();
		for (const std::string& materialFile : materialFiles) {
			Material* standardMaterial = Material::NewPbrMaterial();

			YAML::Node materialNode = YAML::LoadFile(materialFile);
			YAML::Node renderOrderNode = materialNode["RenderOrder"];
			YAML::Node alphaCutoffNode = materialNode["AlphaCutoff"];
			YAML::Node diffuseNode = materialNode["DiffuseTexture"];
			YAML::Node normalNode = materialNode["NormalTexture"];
			YAML::Node metallicNode = materialNode["MetallicTexture"];

			const auto& renderOrder = renderOrderNode.as<std::string>();
			if (renderOrder == "Opaque") {
				standardMaterial->SetRenderOrder(RenderOrder::opaque);
			}
			else if (renderOrder == "Cutout") {
				standardMaterial->SetRenderOrder(RenderOrder::cutout);
			}
			else if (renderOrder == "Transparent") {
				standardMaterial->SetRenderOrder(RenderOrder::transparent);
			}
			else {
				ASSERT(false, "Invalid rendering order");
			}
			
			standardMaterial->SetAlphaCutoff(alphaCutoffNode.as<f32>());

			if (!diffuseNode.IsNull()) {
				const auto& diffuseTextureFile = diffuseNode.as<std::string>();
				Ref<Texture> diffuse = Texture::Load(diffuseTextureFile, Texture::Type::Default);
				standardMaterial->SetAlbedo(diffuse);
			}
			
			if (!normalNode.IsNull()) {
				const auto& normalTextureFile = normalNode.as<std::string>();
				Ref<Texture> normal = Texture::Load(normalTextureFile, Texture::Type::NormalMap);
				standardMaterial->SetNormal(normal);
			}

			if (!metallicNode.IsNull()) {
				const auto& metallicTextureFile = metallicNode.as<std::string>();
				Ref<Texture> metallic = Texture::Load(metallicTextureFile, Texture::Type::Default);
				standardMaterial->SetMetallic(metallic);
			}

			meshRenderer.materials.push_back(standardMaterial);
		}

		if (rootEntity == Entity::Null()) {
			rootEntity = entity;
		}

	}

	return rootEntity;
}

Entity Model::ProcessNode(const std::string& directoryPath, const aiScene* scene, const aiNode* node, Entity parent, Material* mat) {
	auto entity = Registry::Create();
	entity.Add<LocalToWorld>();

	aiVector3t<f32> scale;
	aiVector3t<f32> position;
	aiQuaterniont<f32> rotation;
	node->mTransformation.Decompose(scale, rotation, position);
	
	auto& trans = entity.Add<Transform>();
	trans.position = glm::vec3(position.x, position.y, position.z);
	trans.rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
	trans.scale = glm::vec3(scale.x, scale.y, scale.z);

	if (parent != Entity::Null()) {
		entity.Add<Parent>().entity = parent;
	}

	std::vector<Mesh>* meshes = nullptr;
	std::vector<Material*>* materials = nullptr;

	if (node->mNumMeshes > 0) {
		auto& meshRenderer = entity.Add<MeshRenderer>();
		meshes = &meshRenderer.meshes;
		materials = &meshRenderer.materials;
	}

	for (u32 i = 0; i < node->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		const aiMaterial* meshMaterial = scene->mMaterials[mesh->mMaterialIndex];
		u32 diffuseCount = meshMaterial->GetTextureCount(aiTextureType_DIFFUSE);

		if (diffuseCount > 0) {
			aiString textureName;
			meshMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &textureName);

			std::string texturePath = textureName.C_Str();

			ASSERT(texturePath.find("\\/") == std::string::npos, "Textures need to be in same directory as model");

			texturePath = directoryPath + "/" + texturePath;

			Ref<Texture> diffuse = Texture::Load(texturePath, Texture::Type::Default);

			Material* betterMat = Material::NewPbrMaterial();
			betterMat->SetRenderOrder(diffuse->HasTransparency() ? RenderOrder::transparent : RenderOrder::opaque);
			betterMat->SetAlbedo(diffuse);

			meshes->push_back(Mesh::FromAssimpMesh(mesh));
			materials->push_back(betterMat);
		}
		else {
			Ref<Texture> diffuse = Texture::Load("res/Missing.png", Texture::Type::Default);
			Material* betterMat = Material::NewPbrMaterial();
			betterMat->SetAlbedo(diffuse);

			meshes->push_back(Mesh::FromAssimpMesh(mesh));
			materials->push_back(betterMat);
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
