#pragma once
#include <array>
#include "ecs/Entity.h"
#include "Components/Camera.h"

constexpr size_t FrustrumPointCount = 8;

class CameraSystem {
public:
	static void Update();
    static glm::vec3 CamPos();
    static std::array<glm::vec3, FrustrumPointCount> GetViewFrustrumPoints(float zDist);
	static Entity activeCamEntity;
};
