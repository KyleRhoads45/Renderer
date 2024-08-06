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
    static void EndFrame();
    static void PresentFrame();

    static void NewGizmosFrame();
    static void EndGizmosFrame();
    
    static u32 FrameBufferTexture();
    static glm::i32vec2 GetFrameBufferSize(); 
    static void ResizeFrameBuffer(const glm::i32vec2& size);

    static void DrawMesh(const Mesh& mesh);
    static void DrawMesh(const Mesh& mesh, const LocalToWorld& toWorld, Shader& shader);
    static void DrawMesh(const MeshRenderer& meshRenderer, const LocalToWorld& toWorld, Shader& shader);

    static void DrawFullScreenQuad(const Shader& shader);
    
    static void DebugDrawBounds(glm::vec3* points);
    static void DebugDrawPoint(const glm::vec3 point);

    struct PostProcessingParams {
        f32 contrast;
        f32 saturation;
        f32 exposure;
    };
    
    static PostProcessingParams GetPostProcessingParams() { return m_PostProcessingParams; }
    static void SetPostProcessingParams(const PostProcessingParams& params) { m_PostProcessingParams = params; }
private:
    static void DrawSkybox();
private:
    inline static FrameBuffer m_HdrFrameBuffer;
    inline static FrameBuffer m_SrgbFrameBuffer;
    inline static PostProcessingParams m_PostProcessingParams;
};

