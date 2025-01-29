#include <glad/glad.h>
#include "Model.h"
#include "Renderer.h"
#include "core/CameraSystem.h"
#include "Enviroment.h"
#include "ecs/Registry.h"
#include "ecs/View.h"
#include "ShadowMapper.h"

void ShadowMapper::Init(const u32 textureSize, const f32 shadowDist) {
	m_ShadowMap = DepthTexture(textureSize, textureSize);
	m_ShadowDist = shadowDist;
	m_LightViewProjection = glm::mat4(1.0f);
	m_TextureSize = textureSize;
	m_DepthShader = Shader("src/shaders/Depth.vert", "src/shaders/Depth.frag");

	glGenFramebuffers(1, &m_DepthFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthFrameBuffer);
	m_ShadowMap.AttachToActiveFrameBuffer();

	// Need to explicitly tell OpenGL we're not using color
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapper::PerformShadowPass(Registry& registry) {
	CalculateLightViewProjection();

	i32 viewportDimensions[4];
	glGetIntegerv(GL_VIEWPORT, viewportDimensions);
	i32 viewportWidth = viewportDimensions[2];
	i32 viewportHeight = viewportDimensions[3];

	glViewport(0, 0, m_TextureSize, m_TextureSize);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthFrameBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	m_DepthShader.Bind();
	m_DepthShader.SetMat4("viewProjection", m_LightViewProjection);

	const auto view = View<LocalToWorld, Transform, MeshRenderer>(registry);
	for (const auto entity : view) {
		auto& toWorld = registry.Get<LocalToWorld>(entity);
		const auto& meshRenderer = registry.Get<MeshRenderer>(entity);
		
		m_DepthShader.SetMat4("model", toWorld.matrix);

		for (const auto& mesh : meshRenderer.meshes) {
			glBindVertexArray(mesh.m_Vao);
			glDrawElements(GL_TRIANGLES, mesh.m_NumIndices, GL_UNSIGNED_INT, 0);
		}
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, viewportWidth, viewportHeight);

	Enviroment::Instance()->SetLightViewProjection(m_LightViewProjection);
}

void ShadowMapper::BindShadowMap(const i32 textureUnit) {
	m_ShadowMap.Bind(textureUnit);
}

void ShadowMapper::CalculateLightViewProjection() {
	f32 projSize = CameraSystem::ViewFrustumDiagonal(m_ShadowDist);

	glm::vec3 lightDir = Enviroment::Instance()->GetLightDir();
	glm::vec3 frustumCenter = CameraSystem::ActiveCamPos() + CameraSystem::ActiveCamForward() * (m_ShadowDist / 2.0f);

	// Make the light's view matrix move in texel size increments by snapping the frustum center.
	// This fixes the swimming effect when moving the camera around.
	{
		glm::mat4 lightSpaceView = glm::lookAt(glm::vec3(0, 0, 0), lightDir, glm::vec3(0, 1, 0));

		// Convert camera frustum center to light space so its as if we are viewing it from the lights perspective.
		// Note: this is still in world space so its not always centered at origin. 
		frustumCenter = lightSpaceView * glm::vec4(frustumCenter, 1.0f);

		// Snap the center to the shadow texel grid in world space
		glm::vec3 worldUnitsPerTexel = glm::vec3(projSize / m_TextureSize, projSize / m_TextureSize, 1.0f);
		frustumCenter /= worldUnitsPerTexel;
		frustumCenter = glm::floor(frustumCenter);
		frustumCenter *= worldUnitsPerTexel;

		// Transform it back into world space so its projected properly
		frustumCenter = glm::inverse(lightSpaceView) * glm::vec4(frustumCenter, 1.0f);
	}

	// Create the light's view matrix with the view position being the center of the camera's frustum.
	glm::mat4 view = glm::lookAt(frustumCenter - lightDir, frustumCenter, glm::vec3(0, 1, 0));

	f32 halfProjSize = projSize / 2.0f;

	// When creating the light's projection matrix we extend the depth so that
	// close, but out of view fragments of models don't get discarded by the 
	// tight clip space and create holes in shadows that the camera can see.
	constexpr f32 depthScaler = 2.0f;
	glm::mat4 projection = glm::ortho(-halfProjSize, halfProjSize, -halfProjSize, halfProjSize, -projSize * depthScaler, projSize);

	m_LightViewProjection = projection * view;
}
