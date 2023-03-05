#pragma once
#include "Model.h"
#include "Shader.h"
#include "Enviroment.h"
#include "core/SceneCamera.h"

class Renderer {
public:
    static void Init();
    static void DrawMesh(const Mesh& mesh, const SceneCamera& camera, const Shader& shader);
    static void RenderScene(const SceneCamera& camera);

    static void SetEnviroment(const Enviroment enviorment);
private:
    static Mesh m_SkyboxMesh;
    static Enviroment m_Enviorment;

    static uint32_t m_CamUniformBuffer;
    static uint32_t m_EnviromentUniformBuffer;
};

