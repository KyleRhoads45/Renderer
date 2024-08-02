#include <cassert>
#include <glad/glad.h>
#include "core/Base.h"
#include "core/Primatives.h"
#include "ShadowMapper.h"
#include "core/CameraSystem.h"
#include "ecs/Registry.h"
#include "ecs/View.h"
#include "Renderer.h"

void Renderer::Init() {
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	i32 hardwareTextureUnitCount;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &hardwareTextureUnitCount);
	for (i32 i = 0; i < hardwareTextureUnitCount; i++) {
		glEnable(GL_TEXTURE0 + i);
	}

	s_FrameBuffer = FrameBuffer({960, 540}, FrameBuffer::SRGB);
}

void Renderer::RenderScene() {
	DrawSkybox();
	
	const auto view = View<LocalToWorld, Transform, MeshRenderer>();

    for (const auto entity : view) {
        auto& toWorld = Registry::Get<LocalToWorld>(entity);
        const auto& meshRenderer = Registry::Get<MeshRenderer>(entity);

		for (u32 i = 0; i < meshRenderer.meshes.size(); i++) {
			assert(meshRenderer.materials[i]);
			Material* mat = meshRenderer.materials[i];

			if (mat->GetRenderOrder() == RenderOrder::transparent) continue;

			mat->Bind(toWorld);

			glBindVertexArray(meshRenderer.meshes[i].m_Vao);
			glDrawElements(GL_TRIANGLES, meshRenderer.meshes[i].m_NumIndices, GL_UNSIGNED_INT, nullptr);
		}
    }

	// Draw transparent objects
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (const auto entity : view) {
        auto& toWorld = Registry::Get<LocalToWorld>(entity);
        const auto& meshRenderer = Registry::Get<MeshRenderer>(entity);

		for (u32 i = 0; i < meshRenderer.meshes.size(); i++) {
			assert(meshRenderer.materials[i]);
			Material* mat = meshRenderer.materials[i];

			if (mat->GetRenderOrder() == RenderOrder::opaque) continue;

			mat->Bind(toWorld);

			glBindVertexArray(meshRenderer.meshes[i].m_Vao);
			glDrawElements(GL_TRIANGLES, meshRenderer.meshes[i].m_NumIndices, GL_UNSIGNED_INT, nullptr);
		}
    }

	glDisable(GL_BLEND);
}

void Renderer::NewFrame() {
	ShadowMapper::PerformShadowPass();
	s_FrameBuffer.BeginDraw();
}

void Renderer::ResumeFrame() {
	s_FrameBuffer.ResumeDraw();
}

void Renderer::EndFrame() {
	s_FrameBuffer.EndDraw();
}

void Renderer::PresentFrame() {
	s_FrameBuffer.BlitToScreen();
}

u32 Renderer::FrameBufferTexture() {
	return s_FrameBuffer.Texture();
}

glm::i32vec2 Renderer::GetFrameBufferSize() {
	return s_FrameBuffer.Size();
}

void Renderer::ResizeFrameBuffer(const glm::i32vec2& size) {
	s_FrameBuffer.Resize(size);
}

void Renderer::DrawMesh(const MeshRenderer& meshRenderer, const LocalToWorld& toWorld, Shader& shader) {
	for (const auto& mesh : meshRenderer.meshes) {
		DrawMesh(mesh, toWorld, shader);
	}
}

void Renderer::DrawMesh(const Mesh& mesh, const LocalToWorld& toWorld, Shader& shader) {
	shader.Bind();
	shader.SetMat4("model", toWorld.matrix);
	glBindVertexArray(mesh.m_Vao);
	glDrawElements(GL_TRIANGLES, mesh.m_NumIndices, GL_UNSIGNED_INT, nullptr);
}

void Renderer::DebugDrawBounds(glm::vec3* points) {
	static Shader debugShader("src/shaders/Debug.vert", "src/shaders/Debug.frag");
	static Mesh cube = Primatives::Cube(false);

	debugShader.Bind();
	debugShader.SetMat4("model", glm::mat4(1.0f));

	for (i32 i = 0; i < 8; i++) {
		cube.m_Verts[i].position = points[i];
	}
	cube.UpdateVertexBuffer();

	glBindVertexArray(cube.m_Vao);
	glDrawElements(GL_LINE_LOOP, cube.m_NumIndices, GL_UNSIGNED_INT, nullptr);
}

void Renderer::DebugDrawPoint(const glm::vec3 point) {
	static Shader debugShader("src/shaders/Debug.vert", "src/shaders/Debug.frag");
	static Mesh cube = Primatives::Cube(false);

	debugShader.Bind();

	glm::mat4 model(1.0f);
	model = glm::translate(model, point);
	model = glm::scale(model, glm::vec3(0.1f));

	debugShader.SetMat4("model", model);

	glBindVertexArray(cube.m_Vao);
	glDrawElements(GL_TRIANGLES, cube.m_NumIndices, GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawSkybox() {
	static Mesh skyboxMesh = Primatives::Cube(true);
	static Shader skyboxShader = Shader("src/shaders/Skybox.vert", "src/shaders/Skybox.frag");

	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	skyboxShader.Bind();
	Enviroment::Instance()->BindSkybox(0);

	glBindVertexArray(skyboxMesh.m_Vao);
	glDrawElements(GL_TRIANGLES, skyboxMesh.m_NumIndices, GL_UNSIGNED_INT, nullptr);

	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}
