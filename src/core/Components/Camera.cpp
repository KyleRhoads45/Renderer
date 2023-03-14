#include <iostream>
#include "../Input.h"
#include "Camera.h"

Camera::Camera() {
    m_Yaw = 0.0;
    m_Pitch = 0.0;
    m_Fov = 60.0f;
    m_Aspect = 1920.0f / 1080.0f;
    m_Near = 0.01f;
    m_Far = 500.0f;
    m_View = glm::mat4(1.0f);
    m_Projection = glm::perspective(glm::radians(m_Fov), m_Aspect, m_Near, m_Far);
}

glm::mat4 Camera::ViewProjection() {
    return m_Projection * m_View;
}
