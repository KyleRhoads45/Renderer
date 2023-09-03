#pragma once
#include <array>
#include <memory>
#include "Components.h"
#include "renderer/UniformBuffer.h"
#include "ecs/Entity.h"

constexpr size_t FrustrumPointCount = 8;
using FrustrumPoints = std::array<glm::vec3, FrustrumPointCount>;

class CameraSystem {
public:
	static void Init();
	static void Update();
	static void SetActiveCamera(Camera* camera, Transform* transform);
    static FrustrumPoints& GetViewFrustrumPoints(float zDist);
private:
	static void UpdateUniformBuffer();
private:
	inline static Scope<UniformBuffer> s_UniformBuffer;
	inline static Camera* s_ActiveCamera;
	inline static Transform* s_ActiveCameraTransform;
};
