#include <glm/glm.hpp>
#include "core/Input.h"
#include "SceneCamera.h"

void SceneCamera::Update(const glm::vec2 sceneWindowSize) {
    s_Camera.m_Projection = glm::perspectiveFov(glm::radians(s_Camera.m_Fov), sceneWindowSize.x, sceneWindowSize.y, s_Camera.m_Near, s_Camera.m_Far);

    if (!Input::OnMouseHold(GLFW_MOUSE_BUTTON_2)) return;

    auto& trans = s_Transform;

    const glm::vec2 mouseDeltas = Input::GetMouseDeltas();
    s_Camera.m_Yaw -= mouseDeltas.x * 0.1;
    s_Camera.m_Pitch -= mouseDeltas.y * 0.1;

    if (glm::abs(s_Camera.m_Pitch) > 89.0) {
        s_Camera.m_Pitch = glm::clamp(s_Camera.m_Pitch, -89.0f, 89.0f);
    }

    const glm::quat pitchRot = glm::angleAxis(glm::radians(s_Camera.m_Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::quat yawRot = glm::angleAxis(glm::radians(s_Camera.m_Yaw), glm::vec3(0.0f, 1.0f, 0.0f));

    trans.rotation = yawRot * pitchRot;

    float speed = 0.01f;
    if (Input::OnKeyHold(GLFW_KEY_W)) {
        trans.position = (trans.position + trans.Forward() * speed);
    }
    if (Input::OnKeyHold(GLFW_KEY_S)) {
        trans.position = (trans.position - trans.Forward() * speed);
    }
    if (Input::OnKeyHold(GLFW_KEY_D)) {
        trans.position = (trans.position + trans.Right() * speed);
    }
    if (Input::OnKeyHold(GLFW_KEY_A)) {
        trans.position = (trans.position - trans.Right() * speed);
    }

    s_Camera.m_View = glm::translate(glm::mat4(1.0f), trans.position);
    s_Camera.m_View = s_Camera.m_View * glm::mat4_cast(trans.rotation);
    s_Camera.m_View = glm::inverse(s_Camera.m_View);
}
