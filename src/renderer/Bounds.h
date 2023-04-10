#pragma once
#include <glm/glm.hpp>
#include "core/Base.h"

class Bounds {
public:
	Bounds(glm::vec3* points, u32 size);

	glm::vec3 m_Min;
	glm::vec3 m_Max;
};
