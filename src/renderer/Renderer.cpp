#include <cassert>
#include <glad/glad.h>
#include "Renderer.h"
#include "core/Components.h"
#include "core/Primatives.h"
#include "ecs/Registry.h"
#include "ecs/View.h"

Mesh Renderer::m_SkyboxMesh;
uint32_t Renderer::m_CamUniformBuffer;
uint32_t Renderer::m_EnviromentUniformBuffer;
Enviroment Renderer::m_Enviorment;

void Renderer::Init() {
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glGenBuffers(1, &m_CamUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, m_CamUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 208, NULL, GL_STATIC_DRAW);

	// Bind camera uniform buffer to binding point index 0
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_CamUniformBuffer);

	glGenBuffers(1, &m_EnviromentUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, m_EnviromentUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 32, NULL, GL_STATIC_DRAW);

	// Bind enviroment uniform buffer to binding point index 1
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_EnviromentUniformBuffer);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_SkyboxMesh = Primatives::Cube(true);
}

void Renderer::SetEnviroment(const Enviroment enviorment) {
	m_Enviorment = enviorment;
	glBindBuffer(GL_UNIFORM_BUFFER, m_EnviromentUniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &m_Enviorment.ambientStrength);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 12, &m_Enviorment.ambientColor);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::DrawMesh(const Mesh& mesh, const SceneCamera& camera, const Shader& shader) {
    glm::mat4 model(1.0f);

    shader.Bind();
    shader.SetMat4("model", model);
    shader.SetMat4("view", camera.m_View);
    shader.SetMat4("projection", camera.m_Projection);

    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
}

void Renderer::RenderScene(const SceneCamera& camera) {
	glDepthFunc(GL_LESS);

	glm::mat4 viewProjection = camera.m_Projection * camera.m_View;

	glBindBuffer(GL_UNIFORM_BUFFER, m_CamUniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, &camera.m_Position);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 64, &camera.m_View);
	glBufferSubData(GL_UNIFORM_BUFFER, 80, 64, &camera.m_Projection);
	glBufferSubData(GL_UNIFORM_BUFFER, 144, 64, &viewProjection);

    for (auto entity : View<Transform, MeshRenderer>()) {
        auto transform = entity.GetComponent<Transform>();
        auto meshRenderer = entity.GetComponent<MeshRenderer>();
        
        assert(meshRenderer->material);

        meshRenderer->material->Bind();
        meshRenderer->material->shader.SetMat4("model", transform->Model());

        glBindVertexArray(meshRenderer->mesh.vao);
        glDrawElements(GL_TRIANGLES, meshRenderer->mesh.numIndices, GL_UNSIGNED_INT, 0);
    }

	// Draw Skybox
	glDepthFunc(GL_LEQUAL);

	m_Enviorment.skyboxMaterial->Bind();
	glBindVertexArray(m_SkyboxMesh.vao);
	glDrawElements(GL_TRIANGLES, m_SkyboxMesh.numIndices, GL_UNSIGNED_INT, 0);
}
