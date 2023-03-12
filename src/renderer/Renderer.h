#pragma once
#include "Model.h"
#include "Shader.h"
#include "Enviroment.h"
#include "DepthTexture.h"
#include "Bounds.h"
#include "core/Components.h"
#include "UniformBuffer.h"

class Renderer {
public:
    static void Init();
    static void RenderScene(Camera& camera);
    static void DrawSkybox();
    static void DebugDrawBounds(glm::vec3* points);
    static void DebugDrawPoint(const glm::vec3 point);
private:
    static UniformBuffer cameraUniformBuffer;
};

