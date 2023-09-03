#include "Input.h"
#include "Components.h"

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

bool Camera::operator!=(const Camera& other) const {
    if (m_Fov != other.m_Fov)               return true;
    if (m_Aspect != other.m_Aspect)         return true;
    if (m_Near != other.m_Near)             return true;
    if (m_Far != other.m_Far)               return true;
    if (m_View != other.m_View)             return true;
    if (m_Projection != other.m_Projection) return true;
    return false;
}

glm::mat4 Camera::ViewProjection() const {
    return m_Projection * m_View;
}

glm::vec3 Transform::Forward() const {
	return glm::rotate(rotation, glm::vec3(0, 0, -1));
}

glm::vec3 Transform::Right() const {
	return glm::rotate(rotation, glm::vec3(1, 0, 0));
}

glm::vec3 Transform::Up() const {
	return glm::rotate(rotation, glm::vec3(0, 1, 0));
}
