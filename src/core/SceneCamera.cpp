#include "SceneCamera.h"
#include "Input.h"

SceneCamera::SceneCamera() {
    m_Yaw = 0.0;
    m_Pitch = 0.0;
    m_Position = glm::vec3(0.0f, 0.0f, -10.0f); 
    m_View = glm::translate(glm::mat4(1.0f), m_Position);
    m_Projection = glm::perspective(glm::radians(60.0f), 1920.0f / 1080.0f, 0.01f, 1000.0f);
}

void SceneCamera::Update() {
    if (!Input::OnMouseHold(GLFW_MOUSE_BUTTON_2)) return;

    const glm::vec2 mouseDeltas = Input::GetMouseDeltas();
    m_Yaw += mouseDeltas.x * 0.1;
    m_Pitch += mouseDeltas.y * 0.1;
    if (m_Pitch > 89.0) {
        m_Pitch = 89.0;
    }
    if (m_Pitch < -89.0) {
        m_Pitch = -89.0;
    }

    const glm::quat pitchRot = glm::angleAxis(glm::radians(m_Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::quat yawRot = glm::angleAxis(glm::radians(m_Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::quat rotation = pitchRot * yawRot;
    m_View = glm::mat4_cast(rotation);

    const glm::vec3 forward = glm::rotate(glm::inverse(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
    const glm::vec3 right = glm::rotate(glm::inverse(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
    if (Input::OnKeyHold(GLFW_KEY_W)) {
        m_Position -= forward * 0.1f;
    }
    if (Input::OnKeyHold(GLFW_KEY_S)) {
        m_Position += forward * 0.1f;
    }
    if (Input::OnKeyHold(GLFW_KEY_D)) {
        m_Position -= right * 0.1f;
    }
    if (Input::OnKeyHold(GLFW_KEY_A)) {
        m_Position += right * 0.1f;
    }
    m_View = glm::translate(m_View, m_Position);
}
