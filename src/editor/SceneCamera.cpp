#include <glm/glm.hpp>
#include "core/Input.h"
#include "SceneCamera.h"

void SceneCamera::Update(const glm::vec2 sceneWindowSize) {
    s_Camera.projection = glm::perspectiveFov(glm::radians(s_Camera.fov), sceneWindowSize.x, sceneWindowSize.y, s_Camera.near, s_Camera.far);

    if (!Input::OnMouseHold(GLFW_MOUSE_BUTTON_2)) return;

    auto& trans = s_Transform;

    const glm::vec2 mouseDeltas = Input::GetMouseDeltas();
    s_Camera.yaw -= mouseDeltas.x * 0.1f;
    s_Camera.pitch -= mouseDeltas.y * 0.1f;

    if (glm::abs(s_Camera.pitch) > 89.0f) {
        s_Camera.pitch = glm::clamp(s_Camera.pitch, -89.0f, 89.0f);
    }

    const glm::quat pitchRot = glm::angleAxis(glm::radians(s_Camera.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::quat yawRot = glm::angleAxis(glm::radians(s_Camera.yaw), glm::vec3(0.0f, 1.0f, 0.0f));

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

    s_Camera.view = glm::translate(glm::mat4(1.0f), trans.position);
    s_Camera.view = s_Camera.view * glm::mat4_cast(trans.rotation);
    s_Camera.view = glm::inverse(s_Camera.view);
}
