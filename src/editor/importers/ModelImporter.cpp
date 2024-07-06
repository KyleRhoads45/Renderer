#include <filesystem>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "core/Serializer.h"
#include "renderer/Material.h"
#include "ModelImporter.h"

struct ModelImporter::ProcessData {
	Serializer m_Serializer;
	const aiScene* m_Scene;
	std::vector<std::string> m_MaterialPaths;
	std::string m_DirectoryPath;
};

void ModelImporter::Import(const char* meshPath) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	assert(scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode);

	std::string dirPath = meshPath;
	dirPath = dirPath.substr(0, dirPath.find_last_of("\\/"));

	ProcessData data {
		.m_Scene = scene,
		.m_MaterialPaths = CreateMaterialFiles(scene, dirPath),
		.m_DirectoryPath = std::move(dirPath), // ! Can't use dirPath after this
	};

	data.m_Serializer.BeginMap();
	data.m_Serializer.WriteKeyValue("SourceFile", meshPath);
	data.m_Serializer.EndMap();

	ProcessNode(data, scene->mRootNode, -1);

	std::string importedName = std::string(meshPath) + ".model";
	data.m_Serializer.WriteToFile(importedName);
}

std::vector<std::string> ModelImporter::CreateMaterialFiles(const aiScene* scene, const std::string& dirPath) {
	std::vector<std::string> materialPaths;
	
	for (u32 i = 0; i < scene->mNumMaterials; i++) {
		const aiMaterial* mat = scene->mMaterials[i];
		
		std::string matFileName = dirPath + "/" + std::string(mat->GetName().C_Str()) + ".mat";
		materialPaths.push_back(matFileName);

		Serializer serializer;
		serializer.BeginMap();
		serializer.WriteKeyValue("AlbedoTexture", TextureFromMaterial(*mat, dirPath, aiTextureType_DIFFUSE));
		serializer.WriteKeyValue("NormalTexture", TextureFromMaterial(*mat, dirPath, aiTextureType_NORMALS));
		serializer.WriteKeyValue("MetalRoughTexture", TextureFromMaterial(*mat, dirPath, aiTextureType_METALNESS));
		serializer.WriteKeyValue("AlphaCutoff", 0.0f);
		serializer.WriteKeyValue("Metallicness", 0.5f);
		serializer.WriteKeyValue("Roughness", 0.5f);
		serializer.WriteKeyValue("Specularity", 1.0f);
		serializer.WriteKeyValue("RenderOrder", static_cast<i32>(RenderOrder::opaque));
		serializer.EndMap();
		serializer.WriteToFile(matFileName);
	}

	return materialPaths;
}

void ModelImporter::ProcessNode(ProcessData& data, const aiNode* node, const i32 parentId) {
	Entity entity = Registry::Create();
	entity.Add<LocalToWorld>();

	i32 entityId = BeginEntity(data.m_Serializer, parentId);

	aiVector3t<f32> scale;
	aiVector3t<f32> position;
	aiQuaterniont<f32> rotation;
	node->mTransformation.Decompose(scale, rotation, position);
	
	Transform trans;
	trans.position = glm::vec3(position.x, position.y, position.z);
	trans.rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
	trans.scale = glm::vec3(scale.x, scale.y, scale.z);

	data.m_Serializer.Serialize(trans);

	std::vector<i32> meshIndicies;
	std::vector<std::string> materialPaths;

	for (u32 i = 0; i < node->mNumMeshes; i++) {
		u32 meshIndex = node->mMeshes[i];
		meshIndicies.push_back(meshIndex);

		const aiMesh* mesh = data.m_Scene->mMeshes[meshIndex];
		materialPaths.push_back(data.m_MaterialPaths[mesh->mMaterialIndex]);
	}

	data.m_Serializer.BeginKeyMap("MeshRenderer");
	data.m_Serializer.WriteKeyValue("MeshIndices", meshIndicies);
	data.m_Serializer.WriteKeyValue("Materials", materialPaths);
	data.m_Serializer.EndMap();

	EndEntity(data.m_Serializer);

	for (u32 i = 0; i < node->mNumChildren; i++) {
		ProcessNode(data, node->mChildren[i], entityId);
	}
}

i32 ModelImporter::BeginEntity(Serializer& serializer, const i32 parentId) {
	static i32 entityId = -1;
	entityId++;

	serializer.BeginMap();
	serializer.WriteKeyValue("EntityId", entityId);

	if (parentId >= 0) {
		serializer.WriteKeyValue("ParentId", parentId);
	}
	else {
		serializer.WriteKeyValue("ParentId", YAML::Null);
	}

	serializer.BeginKeyMap("Components");
	return entityId;
}

void ModelImporter::EndEntity(Serializer& serializer) {
	serializer.EndMap();
	serializer.EndMap();
}

std::string ModelImporter::TextureFromMaterial(const aiMaterial& material, const std::string& dirPath, const aiTextureType textureType) {
	aiString textureFileName;
	aiReturn res = material.GetTexture(textureType, 0, &textureFileName);

	if (res == aiReturn_FAILURE) {
		return "";
	}

	std::string texturePath = textureFileName.C_Str();
	texturePath = dirPath + "/" + texturePath;
	return texturePath;
}
