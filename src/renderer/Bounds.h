#pragma once
#include <glm/glm.hpp>
#include "core/Base.h"

class Bounds {
public:
	Bounds(glm::vec3* points, u32 size);

	f32 XLength();
	f32 YLength();
	f32 ZLength();

	glm::vec3 Center();

	glm::vec3 m_Min;
	glm::vec3 m_Max;
};
