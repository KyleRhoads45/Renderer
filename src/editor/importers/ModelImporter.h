#pragma once
#include "core/Base.h"
#include "renderer/Mesh.h"

class Serializer;

class ModelImporter {
public:
	static void Import(const char* meshPath);
private:
	struct ProcessData;
private:
	static std::vector<std::string> CreateMaterialFiles(const aiScene* scene, const std::string& dirPath);
	static void ProcessNode(ProcessData& data, const aiNode* node, const i32 parentId);
	static i32 BeginEntity(Serializer& serializer, const i32 parentId);
	static void EndEntity(Serializer& serializer);
	static std::string TextureFromMaterial(const aiMaterial& material, const std::string& dirPath, const aiTextureType textureType);
};