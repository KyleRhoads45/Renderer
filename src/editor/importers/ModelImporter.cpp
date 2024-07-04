#include <filesystem>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "core/Base.h"
#include "core/Serializer.h"
#include "renderer/Material.h"
#include "ModelImporter.h"

struct ModelImporter::ProcessData {
	const aiScene* m_Scene;
	Serializer m_Serializer;
	std::string m_DirectoryPath;
	std::vector<std::string> m_MaterialPaths;
};

struct ModelImporter::SerializeTextureData {
	YAML::Emitter* m_Emitter;
	std::string m_DirPath;
	const aiMaterial* m_Mat;
};

void ModelImporter::Import(const char* meshPath) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	assert(scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode);

	std::string dirPath = meshPath;
	dirPath = dirPath.substr(0, dirPath.find_last_of("\\/"));
	
	ProcessData data {
		.m_Scene = scene,
		.m_DirectoryPath = std::move(dirPath),
	};

	for (u32 i = 0; i < scene->mNumMaterials; i++) {
		
		const aiMaterial* mat = scene->mMaterials[i];
		std::string matFileName = data.m_DirectoryPath + "/" + std::string(mat->GetName().C_Str()) + ".mat";
		data.m_MaterialPaths.push_back(matFileName);

		YAML::Emitter emitter;
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "RenderOrder" << YAML::Value << "Opaque";
		emitter << YAML::Key << "AlphaCutoff" << YAML::Value << 0.0f;

		SerializeTextureData serializeTextureData {
			.m_Emitter = &emitter,
			.m_DirPath = data.m_DirectoryPath,
			.m_Mat = mat,
		};

		SerializeTexture(serializeTextureData, "DiffuseTexture", aiTextureType_DIFFUSE);
		SerializeTexture(serializeTextureData, "NormalTexture", aiTextureType_NORMALS);
		SerializeTexture(serializeTextureData, "MetallicTexture", aiTextureType_METALNESS);

		emitter << YAML::EndMap;
		Serializer::WriteToFile(emitter, matFileName);
	}

	data.m_Serializer.BeginMap();
	data.m_Serializer.WriteKeyValue("SourceFile", meshPath);
	data.m_Serializer.EndMap();

	ProcessNode(data, scene->mRootNode, -1);

	std::string importedName = std::string(meshPath) + ".model";
	data.m_Serializer.WriteToFile(importedName);
}

void ModelImporter::ProcessNode(ProcessData& data, const aiNode* node, const i32 parentId) {
	Entity entity = Registry::Create();
	entity.Add<LocalToWorld>();

	i32 entityId = data.m_Serializer.BeginEntity(parentId);

	aiVector3t<f32> scale;
	aiVector3t<f32> position;
	aiQuaterniont<f32> rotation;
	node->mTransformation.Decompose(scale, rotation, position);
	
	Transform trans;
	trans.position = glm::vec3(position.x, position.y, position.z);
	trans.rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
	trans.scale = glm::vec3(scale.x, scale.y, scale.z);

	data.m_Serializer.Serialize(trans);

	MeshRendererImport meshRendrImport;

	for (u32 i = 0; i < node->mNumMeshes; i++) {
		u32 meshIndex = node->mMeshes[i];
		meshRendrImport.meshIndicies.push_back(meshIndex);

		const aiMesh* mesh = data.m_Scene->mMeshes[meshIndex];
		meshRendrImport.materialFilePaths.push_back(data.m_MaterialPaths[mesh->mMaterialIndex]);
	}

	data.m_Serializer.Serialize(meshRendrImport);
	data.m_Serializer.EndEntity();

	for (u32 i = 0; i < node->mNumChildren; i++) {
		ProcessNode(data, node->mChildren[i], entityId);
	}
}

void ModelImporter::SerializeTexture(SerializeTextureData& data, const std::string& key, const aiTextureType textureType) {
	*data.m_Emitter << YAML::Key << key;

	aiString textureFileName;
	aiReturn res = data.m_Mat->GetTexture(textureType, 0, &textureFileName);

	if (res == aiReturn_FAILURE) {
		*data.m_Emitter << YAML::Value << YAML::Null;
		return;
	}

	std::string texturePath = textureFileName.C_Str();
	texturePath = data.m_DirPath + "/" + texturePath;
	*data.m_Emitter << YAML::Value << texturePath;
}
