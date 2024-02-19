#pragma once
#include "Model.h"
#include "Shader.h"
#include "Enviroment.h"
#include "DepthTexture.h"
#include "Bounds.h"
#include "core/Components.h"
#include "UniformBuffer.h"
#include "FrameBuffer.h"

class Renderer {
public:
    static void Init();
    static void RenderScene();

    static void PerformAllPrePass();
    static void PerformShadowPass();
    static void PerformSkyboxPass();

    static void DrawMesh(const MeshRenderer& meshRenderer, const LocalToWorld& toWorld, Shader& shader);
    static void DrawMesh(const Mesh& mesh, const LocalToWorld& toWorld, Shader& shader);
    static void DebugDrawBounds(glm::vec3* points);
    static void DebugDrawPoint(const glm::vec3 point);
public:
    inline static FrameBuffer* s_FrameBuffer;
};

