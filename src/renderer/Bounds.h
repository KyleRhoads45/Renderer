#pragma once
#include <glm/glm.hpp>

struct Bounds {
	Bounds(glm::vec3* points, size_t size);

	glm::vec3 min;
	glm::vec3 max;
};
