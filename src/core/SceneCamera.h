#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class SceneCamera {
public:
    SceneCamera();
    void Update();
    glm::mat4 view;
    glm::mat4 projection;

private:
    float yaw;
    float pitch;
    glm::vec3 position;
};
