#pragma once
#include "Model.h"
#include "Shader.h"
#include "core/SceneCamera.h"

class Renderer {
public:
    static void DrawMesh(const Mesh& mesh, const SceneCamera& camera, const Shader& shader);
    static void RenderScene(const SceneCamera& camera);
};

