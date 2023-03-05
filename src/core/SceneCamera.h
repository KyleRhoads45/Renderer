#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class SceneCamera {

public:

    SceneCamera();
    void Update();

    glm::mat4 m_View;
    glm::mat4 m_Projection;
    glm::vec3 m_Position;

    float m_Yaw;
    float m_Pitch;

};
