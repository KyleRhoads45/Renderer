#pragma once
#include <array>
#include <memory>
#include "Components.h"
#include "renderer/UniformBuffer.h"

constexpr size_t frustrumPointCount = 8;
using FrustrumPoints = std::array<glm::vec3, frustrumPointCount>;

class CameraSystem {
public:
	static void Init();
	static void Update();
	static void SetActiveCamera(Camera* camera, Transform* transform);
    static FrustrumPoints GetViewFrustumPoints(f32 zDist);
	static f32 ViewFrustumDiagonal(f32 zDist);
	static glm::vec3 ActiveCamPos();
	static glm::vec3 ActiveCamForward();
	static glm::mat4 ActiveCamViewProjection();
	static glm::mat4 ActiveCamView();
	static glm::mat4 ActiveCamProjection();
private:
	static void UpdateUniformBuffer();
private:
	inline static Scope<UniformBuffer> s_UniformBuffer;
	inline static Camera* s_ActiveCamera;
	inline static Transform* s_ActiveCameraTransform;
};
