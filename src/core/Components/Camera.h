#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();

    glm::mat4 m_View;
    glm::mat4 m_Projection;

    float m_Yaw;
    float m_Pitch;
};
