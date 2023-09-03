#include <cassert>
#include <glad/glad.h>
#include "core/Base.h"
#include "core/Primatives.h"
#include "ShadowMapper.h"
#include "core/CameraSystem.h"
#include "ecs/Registry.h"
#include "editor/Editor.h"
#include "ecs/View.h"
#include "Renderer.h"

void Renderer::Init() {
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void Renderer::RenderScene() {
	if (s_FrameBuffer == nullptr) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	ShadowMapper::PerformShadowPass();

	if (s_FrameBuffer != nullptr) {
		s_FrameBuffer->Bind();
	}

	DrawSkybox();

	const auto view = View<Transform, MeshRenderer>();
    for (const auto entity : view) {
        auto& transform = Registry::Get<Transform>(entity);
        auto& meshRenderer = Registry::Get<MeshRenderer>(entity);
        
        assert(meshRenderer.material);
		Material* mat = meshRenderer.material;

        mat->Bind(transform);

        glBindVertexArray(meshRenderer.mesh.vao);
        glDrawElements(GL_TRIANGLES, meshRenderer.mesh.numIndices, GL_UNSIGNED_INT, 0);
    }

	if (s_FrameBuffer != nullptr) {
		s_FrameBuffer->UnBind();
	}
}

void Renderer::DrawMesh(MeshRenderer& meshRenderer, Transform& transform, Shader& shader) {
	shader.Bind();
	shader.SetMat4("model", transform.model);
	glBindVertexArray(meshRenderer.mesh.vao);
	glDrawElements(GL_TRIANGLES, meshRenderer.mesh.numIndices, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawSkybox() {
	static Mesh skyboxMesh = Primatives::Cube(true);
	static Shader skyboxShader = Shader("src/shaders/Skybox.vert", "src/shaders/Skybox.frag");

	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	skyboxShader.Bind();
	glActiveTexture(GL_TEXTURE0);
	Enviroment::Instance()->m_Skybox->Bind();

	glBindVertexArray(skyboxMesh.vao);
	glDrawElements(GL_TRIANGLES, skyboxMesh.numIndices, GL_UNSIGNED_INT, 0);

	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}

void Renderer::DebugDrawBounds(glm::vec3* points) {
	static Shader debugShader("src/shaders/Debug.vert", "src/shaders/Debug.frag");
	static Mesh cube = Primatives::Cube(false);

	debugShader.Bind();
	debugShader.SetMat4("model", glm::mat4(1.0f));

	for (i32 i = 0; i < 8; i++) {
		cube.verts[i].position = points[i];
	}
	cube.UpdateVertexBuffer();

	glBindVertexArray(cube.vao);
	glDrawElements(GL_LINE_LOOP, cube.numIndices, GL_UNSIGNED_INT, 0);
}

void Renderer::DebugDrawPoint(const glm::vec3 point) {
	static Shader debugShader("src/shaders/Debug.vert", "src/shaders/Debug.frag");
	static Mesh cube = Primatives::Cube(false);

	debugShader.Bind();

	glm::mat4 model(1.0);
	model = glm::translate(model, point);
	model = glm::scale(model, glm::vec3(0.1));

	debugShader.SetMat4("model", model);

	glBindVertexArray(cube.vao);
	glDrawElements(GL_TRIANGLES, cube.numIndices, GL_UNSIGNED_INT, 0);
}
