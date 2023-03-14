#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "ecs/Entity.h"

class Transform {
public:
    void SetPosition(glm::vec3 pos);
    void SetRotation(glm::quat rot);
    void SetScale(glm::vec3 scl);
    void Set(glm::vec3 pos, glm::quat rot, glm::vec3 scl);

    // Transforms pos from local space to world space
    glm::vec3 TranformPosition(const glm::vec3& pos);

    // Transforms pos from world space to local space
    glm::vec3 InverseTransformPosition(const glm::vec3& pos);

    uint32_t GetChildCount();
    Transform& GetChild(const uint32_t index);
    void AddChild(Transform* child);
    void SetParent(Transform* newParent);

    glm::vec3 Position();
    glm::quat Rotation();
    glm::vec3 Scale();

    glm::vec3 LocalPosition();
    glm::quat LocalRotation();
    glm::vec3 LocalScale();

    glm::vec3 Forward();
    glm::vec3 Right();
    glm::vec3 Up();

    glm::mat4 Model();

    Entity entity;
private:
    void RecalculateModel();
    void UpdateModelRecursive(glm::mat4 parentModel);

    // Local Coordinates if Child
    // Global Coordinates if Root
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::quat rotation;
    glm::vec3 scale = glm::vec3(1, 1, 1);

    // Global Matrix 
    glm::mat4 model;

    Transform* parent = nullptr;
    std::vector<Transform*> children;
};

