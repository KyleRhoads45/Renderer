#include <glm/gtx/string_cast.hpp>
#include "ecs/View.h"
#include "Input.h"
#include "Components.h"
#include "CameraSystem.h"

void CameraSystem::Init() {
	m_UniformBuffer = MakeBox<UniformBuffer>(0);
	m_UniformBuffer->Register("camPos", sizeof(glm::vec3));
	m_UniformBuffer->Register("view", sizeof(glm::mat4));
	m_UniformBuffer->Register("projection", sizeof(glm::mat4));
	m_UniformBuffer->Register("viewProjection", sizeof(glm::mat4));
	m_UniformBuffer->FinishedRegistering();
}

void CameraSystem::Update() {
    if (!Input::OnMouseHold(GLFW_MOUSE_BUTTON_2)) return;

    auto trans = Registry::Get<Transform>(activeCamEntity);
    auto cam = Registry::Get<Camera>(activeCamEntity);

    const glm::vec2 mouseDeltas = Input::GetMouseDeltas();
    cam->m_Yaw -= mouseDeltas.x * 0.1;
    cam->m_Pitch -= mouseDeltas.y * 0.1;

    if (glm::abs(cam->m_Pitch) > 89.0) {
        cam->m_Pitch = glm::clamp(cam->m_Pitch, -89.0f, 89.0f);
    }

    const glm::quat pitchRot = glm::angleAxis(glm::radians(cam->m_Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::quat yawRot = glm::angleAxis(glm::radians(cam->m_Yaw), glm::vec3(0.0f, 1.0f, 0.0f));

    trans->SetRotation(yawRot * pitchRot);

    if (Input::OnKeyHold(GLFW_KEY_W)) {
        trans->SetPosition(trans->Position() + trans->Forward() * 0.1f);
    }
    if (Input::OnKeyHold(GLFW_KEY_S)) {
        trans->SetPosition(trans->Position() - trans->Forward() * 0.1f);
    }
    if (Input::OnKeyHold(GLFW_KEY_D)) {
        trans->SetPosition(trans->Position() + trans->Right() * 0.1f);
    }
    if (Input::OnKeyHold(GLFW_KEY_A)) {
        trans->SetPosition(trans->Position() - trans->Right() * 0.1f);
    }

    cam->m_View = glm::translate(glm::mat4(1.0f), trans->Position());
    cam->m_View = cam->m_View * glm::mat4_cast(trans->Rotation());
    cam->m_View = glm::inverse(cam->m_View);

    UpdateUniformBuffer(trans, cam);
}

void CameraSystem::SetActiveCameraEntity(Entity entity) {
    activeCamEntity = entity;
    auto trans = Registry::Get<Transform>(activeCamEntity);
    auto cam = Registry::Get<Camera>(activeCamEntity);
    UpdateUniformBuffer(trans, cam);
}

std::array<glm::vec3, FrustrumPointCount>&
CameraSystem::GetViewFrustrumPoints(float zDist = 0.0f) {
    auto trans = Registry::Get<Transform>(activeCamEntity);
    auto cam = Registry::Get<Camera>(activeCamEntity);

    f32 farDist = (zDist == 0.0f) ? cam->m_Far : zDist;
    f32 nearDist = cam->m_Near;
    f32 aspect = cam->m_Aspect;
    f32 pheta = glm::radians(cam->m_Fov / 2.0f);

    const glm::vec3 pos = trans->Position();
    const glm::vec3 forward = trans->Forward();
    const glm::vec3 right = trans->Right();
    const glm::vec3 up = trans->Up();

    // Calculate far plane positions

    f32 halfFarPlaneHeight = (glm::tan(pheta) * farDist);
    f32 halfFarPlaneWidth = halfFarPlaneHeight * aspect;

    const glm::vec3 farUp = up * halfFarPlaneHeight;
    const glm::vec3 farRight = right * halfFarPlaneWidth;
    const glm::vec3 farCenter = pos + (forward * farDist);

    glm::vec3 fTopRight = farCenter + farRight + farUp;
    glm::vec3 fBottomRight = farCenter + farRight - farUp;
    glm::vec3 fTopLeft = farCenter - farRight + farUp;
    glm::vec3 fBottomLeft = farCenter - farRight - farUp;

    // Calculate near plane positions

    f32 halfNearPlaneHeight = (glm::tan(pheta) * nearDist);
    f32 halfNearPlaneWidth = halfNearPlaneHeight * aspect;

    const glm::vec3 nearUp = up * halfNearPlaneHeight;
    const glm::vec3 nearRight = right * halfNearPlaneWidth;
    const glm::vec3 nearCenter = pos + (forward * nearDist);

    glm::vec3 nTopRight = nearCenter + nearRight + nearUp;
    glm::vec3 nBottomRight = nearCenter + nearRight - nearUp;
    glm::vec3 nTopLeft = nearCenter - nearRight + nearUp;
    glm::vec3 nBottomLeft = nearCenter - nearRight - nearUp;

    std::array<glm::vec3, FrustrumPointCount> points {
        fTopRight, fBottomRight, fTopLeft, fBottomLeft,
        nTopRight, nBottomRight, nTopLeft, nBottomLeft
    };

    return points;
}

void CameraSystem::UpdateUniformBuffer(Transform* trans, Camera* cam) {
    glm::vec3 camPos = trans->Position();
    glm::mat4 viewProjection = cam->ViewProjection();

	m_UniformBuffer->SubBufferData("camPos", &camPos);
	m_UniformBuffer->SubBufferData("view", &cam->m_View);
	m_UniformBuffer->SubBufferData("projection", &cam->m_Projection);
	m_UniformBuffer->SubBufferData("viewProjection", &viewProjection);
}
