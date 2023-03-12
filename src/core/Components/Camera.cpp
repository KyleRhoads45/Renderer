#include <iostream>
#include "../Input.h"
#include "Camera.h"

Camera::Camera() {
    m_Yaw = 0.0;
    m_Pitch = 0.0;
    m_View = glm::mat4(1.0f);
    m_Projection = glm::perspective(glm::radians(60.0f), 1920.0f / 1080.0f, 0.01f, 100.0f);
}
