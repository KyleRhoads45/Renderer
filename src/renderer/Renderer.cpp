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

	m_HdrFrameBuffer = FrameBuffer({960, 540}, FrameBuffer::HDR);
	m_SrgbFrameBuffer = FrameBuffer({960, 540}, FrameBuffer::SRGB);
	m_PostProcessingParams = { 0.07f, 0.1f, 0.0f };
}

void Renderer::RenderScene(Registry& registry) {
	DrawSkybox();
	
	const auto view = View<LocalToWorld, Transform, MeshRenderer>(registry);

    for (const auto entity : view) {
        auto& toWorld = registry.Get<LocalToWorld>(entity);
        const auto& meshRenderer = registry.Get<MeshRenderer>(entity);

		for (u32 i = 0; i < meshRenderer.meshes.size(); i++) {
			assert(meshRenderer.materials[i]);
			Material* mat = meshRenderer.materials[i];

			if (mat->GetRenderOrder() == RenderOrder::transparent) continue;

			mat->Bind(toWorld);
			DrawMesh(meshRenderer.meshes[i]);
		}
    }

	// Draw transparent objects
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (const auto entity : view) {
        auto& toWorld = registry.Get<LocalToWorld>(entity);
        const auto& meshRenderer = registry.Get<MeshRenderer>(entity);

		for (u32 i = 0; i < meshRenderer.meshes.size(); i++) {
			assert(meshRenderer.materials[i]);
			Material* mat = meshRenderer.materials[i];

			if (mat->GetRenderOrder() == RenderOrder::opaque) continue;

			mat->Bind(toWorld);
			DrawMesh(meshRenderer.meshes[i]);
		}
    }

	glDisable(GL_BLEND);
}

void Renderer::NewFrame(Registry& registry) {
	ShadowMapper::PerformShadowPass(registry);
	m_HdrFrameBuffer.BindAndClear();
}

void Renderer::EndFrame() {
	m_HdrFrameBuffer.Unbind();
	m_SrgbFrameBuffer.BindAndClear();
	
	static Shader postProcessingShader("src/shaders/PostProcessing.vert", "src/shaders/PostProcessing.frag");

	postProcessingShader.Bind();
	postProcessingShader.SetInt("hdrTexture", 0);
	postProcessingShader.SetFloat("contrast", m_PostProcessingParams.contrast);
	postProcessingShader.SetFloat("saturation", m_PostProcessingParams.saturation);
	postProcessingShader.SetFloat("exposure", m_PostProcessingParams.exposure);
	
	m_HdrFrameBuffer.BindTexture(0);
	DrawFullScreenQuad(postProcessingShader);
	m_SrgbFrameBuffer.Unbind();
}

void Renderer::PresentFrame() {
	m_SrgbFrameBuffer.BlitToScreen();
}

void Renderer::NewGizmosFrame() {
	m_SrgbFrameBuffer.Bind();
}

void Renderer::EndGizmosFrame() {
	m_SrgbFrameBuffer.Unbind();
}

u32 Renderer::FrameBufferTexture() {
	return m_SrgbFrameBuffer.Texture();
}

glm::i32vec2 Renderer::GetFrameBufferSize() {
	return m_SrgbFrameBuffer.Size();
}

void Renderer::ResizeFrameBuffer(const glm::i32vec2& size) {
	m_HdrFrameBuffer.Resize(size);
	m_SrgbFrameBuffer.Resize(size);
}

void Renderer::DrawMesh(const Mesh& mesh) {
	glBindVertexArray(mesh.m_Vao);
	glDrawElements(GL_TRIANGLES, mesh.m_NumIndices, GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawMesh(const MeshRenderer& meshRenderer, const LocalToWorld& toWorld, Shader& shader) {
	for (const auto& mesh : meshRenderer.meshes) {
		DrawMesh(mesh, toWorld, shader);
	}
}

void Renderer::DrawFullScreenQuad(const Shader& shader) {
	static Mesh presentPlane = Primatives::Plane();
	shader.Bind();
	glBindVertexArray(presentPlane.m_Vao);
	glDrawElements(GL_TRIANGLES, presentPlane.m_NumIndices, GL_UNSIGNED_INT, nullptr);
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
