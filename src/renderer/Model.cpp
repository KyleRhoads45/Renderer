#include <filesystem>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "core/Base.h"
#include "core/Serializer.h"
#include "Model.h"

Entity Model::Instantiate(const char* importedModelFile, Registry& registry) {
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

		Entity entity = registry.Create();
		entityLookUp.push_back(entity);
		registry.Add<LocalToWorld>(entity);

		YAML::Node components = node["Components"];
		YAML::Node transNode = components["Transform"];

		auto& trans = registry.Add<Transform>(entity);
		Serializer::Deserialize(transNode, trans);

		if (YAML::Node parentNode = node["ParentId"]; !parentNode.IsNull()) {
			i32 parentIndex = parentNode.as<i32>();
			Entity parentEntity = entityLookUp[parentIndex];

			registry.Add<Parent>(entity).entity = parentEntity;
			registry.GetAdd<Children>(parentEntity).entities.push_back(entity);
		}

		YAML::Node meshRendrNode = components["MeshRenderer"];
		YAML::Node meshIndicesNode = meshRendrNode["MeshIndices"];
		YAML::Node materialsNode = meshRendrNode["Materials"];

		if (meshIndicesNode.IsNull() || materialsNode.IsNull()) continue;

		auto& meshRenderer = registry.Add<MeshRenderer>(entity);

		const auto& meshIndicies = meshIndicesNode.as<std::vector<i32>>();
		for (const i32 meshIndex : meshIndicies) {
			meshRenderer.meshes.push_back(Mesh::FromAssimpMesh(scene->mMeshes[meshIndex]));
		}

		const auto& materialFiles = materialsNode.as<std::vector<std::string>>();
		for (const std::string& materialFile : materialFiles) {
			ASSERT(std::filesystem::exists(materialFile), materialFile);
			Material* standardMaterial = Material::NewPbrMaterial();
			Serializer::Deserialize(materialFile, *standardMaterial);
			meshRenderer.materials.push_back(standardMaterial);
		}

		if (rootEntity == Entity::Null()) {
			rootEntity = entity;
		}

	}

	return rootEntity;
}
