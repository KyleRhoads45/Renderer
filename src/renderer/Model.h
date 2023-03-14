#pragma once
#include <glm/glm.hpp>
#include "ecs/Entity.h"
#include "Mesh.h"
#include "Material.h"

class Model {
public:
	static Entity Instantiate(const char* meshPath, Material* mat);
private:
	static Entity ProcessNode(const aiScene* scene, const aiNode* node, Transform* parent, Material* mat);
};