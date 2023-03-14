#pragma once
#include <array>
#include <memory>
#include "ecs/Entity.h"
#include "Components/Camera.h"
#include "renderer/UniformBuffer.h"

constexpr size_t FrustrumPointCount = 8;

class CameraSystem {
public:
	static void Init();
	static void Update();
	static void SetActiveCameraEntity(Entity entity);
    static std::array<glm::vec3, FrustrumPointCount> GetViewFrustrumPoints(float zDist);
private:
	static void UpdateUniformBuffer(Transform* trans, Camera* cam);
	static std::unique_ptr<UniformBuffer> m_UniformBuffer;
	static Entity activeCamEntity;
};
