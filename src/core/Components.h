#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "renderer/Model.h"
#include "renderer/Shader.h"

struct Transform {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    glm::mat4 model;

    uint32_t childCount = 0;
    std::shared_ptr<Transform* []> children;

    void SetAll(glm::vec3 p, glm::quat r, glm::vec3 s);
    void SetRotation(glm::quat rot);
    void UpdateModel(glm::mat4 parentModel);
};

struct MeshRenderer {
    Mesh mesh;
    Shader shader;
};
