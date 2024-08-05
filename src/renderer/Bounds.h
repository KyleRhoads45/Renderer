#pragma once
#include <glm/glm.hpp>
#include "core/Base.h"

class Bounds {
public:
	Bounds(const glm::vec3* points, const size_t size);

	f32 XLength() const { return m_Max.x - m_Min.x; }
	f32 YLength() const { return m_Max.y - m_Min.y; }
	f32 ZLength() const { return m_Max.z - m_Min.z; }
	f32 MaxLength() const;
	
	glm::vec3 Center() const { return (m_Max + m_Min) / 2.0f; }

	glm::vec3 m_Min;
	glm::vec3 m_Max;
};