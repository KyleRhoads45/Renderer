#pragma once
#include "core/Components.h"

class SceneCamera {
public:
	static void Update(const glm::vec2 sceneWindowSize);
	inline static Camera s_Camera;
	inline static Transform s_Transform;
};

