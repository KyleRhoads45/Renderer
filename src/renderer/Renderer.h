#pragma once
#include "Model.h"
#include "Shader.h"
#include "Enviroment.h"
#include "core/Components.h"
#include "FrameBuffer.h"

class Renderer {
public:
    static void Init();
    static void RenderScene();
    static void NewFrame();
    static void ResumeFrame();
    static void EndFrame();
    static void PresentFrame();
    
    static u32 FrameBufferTexture();
    static glm::i32vec2 GetFrameBufferSize(); 
    static void ResizeFrameBuffer(const glm::i32vec2& size);

    static void DrawMesh(const MeshRenderer& meshRenderer, const LocalToWorld& toWorld, Shader& shader);
    static void DrawMesh(const Mesh& mesh, const LocalToWorld& toWorld, Shader& shader);
    static void DebugDrawBounds(glm::vec3* points);
    static void DebugDrawPoint(const glm::vec3 point);
private:
    static void DrawSkybox();
private:
    inline static FrameBuffer s_FrameBuffer;
};

