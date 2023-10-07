#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/Mesh.h"
#include "core/Base.h"
#include "ecs/Registry.h"

class Model;
class Material;

struct Camera {
    Camera();
    glm::mat4 ViewProjection() const;
    
    bool operator==(const Camera& other) const;
    bool operator!=(const Camera& other) const;

    f32 yaw;
    f32 pitch;
    f32 fov;
    f32 aspect;
    f32 near;
    f32 far;

    glm::mat4 view;
    glm::mat4 projection;
};

struct MeshRenderer {
    Mesh mesh;
    Material* material;
};

struct Transform {
    glm::vec3 Forward() const;
    glm::vec3 Right() const;
    glm::vec3 Up() const;

    // Local or Global
    glm::vec3 position = glm::vec3(0);
    glm::quat rotation = glm::quat();
    glm::vec3 scale    = glm::vec3(1);
};

struct LocalToWorld {
    glm::mat4 matrix;
};

struct Parent {
    Entity entity;
};

struct Children {
    std::vector<Entity> entities;
};