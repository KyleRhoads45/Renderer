#pragma once
#include "Mesh.h"
#include "Material.h"
#include "core/Components.h"

class Model {
public:
	static Entity Instantiate(const char* meshPath, Material* mat);
private:
	static Entity ProcessNode(const aiScene* scene, const aiNode* node, Transform* parent, Material* mat);
};
