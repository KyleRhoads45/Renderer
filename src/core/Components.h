#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "renderer/Mesh.h"
#include "core/Base.h"
#include "ecs/Entity.h"

class Model;
class Material;

struct Camera {
    Camera();
    bool operator!=(const Camera& other) const;
    glm::mat4 ViewProjection() const;

    f32 m_Yaw;
    f32 m_Pitch;
    f32 m_Fov;
    f32 m_Aspect;
    f32 m_Near;
    f32 m_Far;

    glm::mat4 m_View;
    glm::mat4 m_Projection;
};

struct MeshRenderer {
    Mesh mesh;
    Material* material;
};

struct Transform {
    glm::vec3 Forward() const;
    glm::vec3 Right() const;
    glm::vec3 Up() const;

    // Local Coordinates if Child, Global Coordinates if Root
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::quat rotation;
    glm::vec3 scale = glm::vec3(1, 1, 1);

    // Global Matrix 
    glm::mat4 model;
};


