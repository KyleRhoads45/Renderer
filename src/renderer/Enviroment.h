#pragma once
#include <memory>

struct Enviroment {
	std::shared_ptr<Material> skyboxMaterial;

	float ambientStrength;
	glm::vec3 ambientColor;
};
