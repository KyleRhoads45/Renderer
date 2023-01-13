#include <glad/glad.h>
#include "Renderer.h"
#include "core/Components.h"
#include "ecs/Registry.h"
#include "ecs/View.h"

void Renderer::DrawMesh(const Mesh& mesh, const SceneCamera& camera, const Shader& shader) {
    glm::mat4 model(1.0f);
    shader.SetUniformMat4("model", model);
    shader.SetUniformMat4("view", camera.view);
    shader.SetUniformMat4("projection", camera.projection);

    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
}

void Renderer::RenderScene(const SceneCamera& camera) {
    for (auto entity : View<Transform, MeshRenderer>()) {
        auto transform = entity.GetComponent<Transform>();
        auto meshRenderer = entity.GetComponent<MeshRenderer>();
        
        meshRenderer->shader.SetUniformMat4("model", transform->Model());
        meshRenderer->shader.SetUniformMat4("view", camera.view);
        meshRenderer->shader.SetUniformMat4("projection", camera.projection);

        glBindVertexArray(meshRenderer->mesh.vao);
        glDrawElements(GL_TRIANGLES, meshRenderer->mesh.numIndices, GL_UNSIGNED_INT, 0);
    }
}
