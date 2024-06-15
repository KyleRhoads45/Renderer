#pragma once
#include "renderer/Mesh.h"

class ModelImporter {
public:
	static void Import(const char* meshPath);
private:
	struct ProcessData;
	struct SerializeTextureData;
private:
	static void ProcessNode(ProcessData& data, const aiNode* node, const i32 parentId);
	static void SerializeTexture(SerializeTextureData& data, const std::string& key, const aiTextureType textureType);
};