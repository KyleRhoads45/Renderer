#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/Base.h"

class Camera {
public:
    Camera();
    glm::mat4 ViewProjection();

    f32 m_Yaw;
    f32 m_Pitch;
    f32 m_Fov;
    f32 m_Aspect;
    f32 m_Near;
    f32 m_Far;

    glm::mat4 m_View;
    glm::mat4 m_Projection;
};
