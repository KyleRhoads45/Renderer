#pragma once
#include "Mesh.h"
#include "Material.h"
#include "ecs/Registry.h"
#include "core/Components.h"

class Model {
public:
	static Entity Instantiate(const char* importedModelFile);
private:
	static Entity ProcessNode(const std::string& directoryPath, const aiScene* scene, const aiNode* node, const Entity parent, Material* mat);
};
