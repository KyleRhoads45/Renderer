#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "core/Base.h"
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

    u32 GetChildCount();
    Transform& GetChild(const u32 index);
    void AddChild(Transform* child);
    void SetParent(Transform* newParent);

    glm::vec3 Position() const;
    glm::quat Rotation() const;
    glm::vec3 Scale() const;

    glm::vec3 LocalPosition() const;
    glm::quat LocalRotation() const;
    glm::vec3 LocalScale() const;

    glm::vec3 Forward() const;
    glm::vec3 Right() const;
    glm::vec3 Up() const;

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

