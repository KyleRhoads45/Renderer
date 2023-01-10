#include "SceneCamera.h"
#include "Input.h"

SceneCamera::SceneCamera() {
    yaw = 0.0;
    pitch = 0.0;
    position = glm::vec3(0.0f, 0.0f, -10.0f); 
    view = glm::translate(glm::mat4(1.0f), position);
    projection = glm::perspective(glm::radians(60.0f), 1920.0f / 1080.0f, 0.01f, 1000.0f);
}

void SceneCamera::Update() {
    if (!Input::OnMouseHold(GLFW_MOUSE_BUTTON_2)) return;

    const glm::vec2 mouseDeltas = Input::GetMouseDeltas();
    yaw += mouseDeltas.x * 0.1;
    pitch += mouseDeltas.y * 0.1;
    if (pitch > 89.0) {
        pitch = 89.0;
    }
    if (pitch < -89.0) {
        pitch = -89.0;
    }

    const glm::quat pitchRot = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::quat yawRot = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::quat rotation = pitchRot * yawRot;
    view = glm::mat4_cast(rotation);

    const glm::vec3 forward = glm::rotate(glm::inverse(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
    const glm::vec3 right = glm::rotate(glm::inverse(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
    if (Input::OnKeyHold(GLFW_KEY_W)) {
        position -= forward * 0.1f;
    }
    if (Input::OnKeyHold(GLFW_KEY_S)) {
        position += forward * 0.1f;
    }
    if (Input::OnKeyHold(GLFW_KEY_D)) {
        position -= right * 0.1f;
    }
    if (Input::OnKeyHold(GLFW_KEY_A)) {
        position += right * 0.1f;
    }
    view = glm::translate(view, position);
}

