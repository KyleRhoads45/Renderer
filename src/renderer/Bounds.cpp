#include "Bounds.h"

Bounds::Bounds(glm::vec3* points, u32 size) {
	m_Min = points[0];
	m_Max = points[0];

	for (u32 i = 0; i < size; i++) {
		glm::vec3 pos = points[i];

		if (pos.x < m_Min.x) {
			m_Min.x = pos.x;
		}
		if (pos.y < m_Min.y) {
			m_Min.y = pos.y;
		}
		if (pos.z < m_Min.z) {
			m_Min.z = pos.z;
		}

		if (pos.x > m_Max.x) {
			m_Max.x = pos.x;
		}
		if (pos.y > m_Max.y) {
			m_Max.y = pos.y;
		}
		if (pos.z > m_Max.z) {
			m_Max.z = pos.z;
		}
	}
}
