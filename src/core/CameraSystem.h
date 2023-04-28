#pragma once
#include <array>
#include <memory>
#include "Base.h"
#include "ecs/Entity.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "renderer/UniformBuffer.h"

constexpr size_t FrustrumPointCount = 8;

class CameraSystem {
public:
	static void Init();
	static void Update();
	static void SetActiveCameraEntity(Entity entity);
	static const Transform& GetActiveTransform();
    static std::array<glm::vec3, FrustrumPointCount>& GetViewFrustrumPoints(float zDist);
private:
	static void UpdateUniformBuffer(Transform* trans, Camera* cam);
	inline static Scope<UniformBuffer> m_UniformBuffer;
	inline static Entity activeCamEntity;
};
