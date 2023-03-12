#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();

    float m_Yaw;
    float m_Pitch;
    float m_Fov;
    float m_Aspect;
    float m_Near;
    float m_Far;

    glm::mat4 m_View;
    glm::mat4 m_Projection;
};
