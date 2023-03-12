#include "Bounds.h"

Bounds::Bounds(glm::vec3* points, size_t size) 
	: min(points[0]), max(points[0]) {

	for (int i = 0; i < size; i++) {
		glm::vec3 pos = points[i];

		if (pos.x < min.x) {
			min.x = pos.x;
		}
		if (pos.y < min.y) {
			min.y = pos.y;
		}
		if (pos.z < min.z) {
			min.z = pos.z;
		}

		if (pos.x > max.x) {
			max.x = pos.x;
		}
		if (pos.y > max.y) {
			max.y = pos.y;
		}
		if (pos.z > max.z) {
			max.z = pos.z;
		}
	}
}
