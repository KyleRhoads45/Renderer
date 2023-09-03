#include <glad/glad.h>
#include "Model.h"
#include "Renderer.h"
#include "core/CameraSystem.h"
#include "Enviroment.h"
#include "ecs/Registry.h"
#include "ecs/View.h"
#include "ShadowMapper.h"

void ShadowMapper::Init(u32 textureSize, u32 shadowDist) {
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

void ShadowMapper::PerformShadowPass() {
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

	const auto view = View<Transform, MeshRenderer>();
    for (auto entity : view) {
        auto& transform = Registry::Get<Transform>(entity);
        auto& meshRenderer = Registry::Get<MeshRenderer>(entity);
        
		m_DepthShader.SetMat4("model", transform.model);

        glBindVertexArray(meshRenderer.mesh.vao);
        glDrawElements(GL_TRIANGLES, meshRenderer.mesh.numIndices, GL_UNSIGNED_INT, 0);
    }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, viewportWidth, viewportHeight);

	Enviroment::Instance()->SetLightViewProjection(m_LightViewProjection);
}

void ShadowMapper::CalculateLightViewProjection() {
	auto& frustrumPoints = CameraSystem::GetViewFrustrumPoints(m_ShadowDist);

	glm::vec3 lightDir = Enviroment::Instance()->GetLightDir();
	glm::mat4 lightSpaceView = glm::lookAt(glm::vec3(0, 0, 0), lightDir, glm::vec3(0, 1, 0));

	// Convert camera frustrum points to light space
	// its as if we are viewing the frustrum from the lights point of view.
	// Note: this is still in world space so its not always centered at origin. 
	for (i32 i = 0; i < frustrumPoints.size(); i++) {
		frustrumPoints[i] = lightSpaceView * glm::vec4(frustrumPoints[i], 1.0f);
	}

	Bounds bounds(frustrumPoints.data(), frustrumPoints.size());

	f32 width = bounds.XLength();
	f32 height = bounds.YLength();
	f32 depth = bounds.ZLength();

	// Make the light's projection move to texel size increments.
	// This fixes the shimmering effect when translating the camera.
	glm::vec3 worldUnitsPerTexel = glm::vec3(width / m_TextureSize, height / m_TextureSize, 1.0f);

	bounds.m_Min /= worldUnitsPerTexel;
	bounds.m_Min = glm::floor(bounds.m_Min);
	bounds.m_Min *= worldUnitsPerTexel;

	bounds.m_Max /= worldUnitsPerTexel;
	bounds.m_Max = glm::floor(bounds.m_Max);
	bounds.m_Max *= worldUnitsPerTexel;

	// Transform the min and max points from light space into world space.
	// Calculate the center of the bounding box in world space. 
	glm::vec3 worldSpaceMin = glm::inverse(lightSpaceView) * glm::vec4(bounds.m_Min, 1.0f);
	glm::vec3 worldSpaceMax = glm::inverse(lightSpaceView) * glm::vec4(bounds.m_Max, 1.0f);
	glm::vec3 center = (worldSpaceMin + worldSpaceMax) / 2.0f;

	// Create the light's view matrix with the view position being
	// the center of the bounding box.
	glm::mat4 view = glm::lookAt(center - lightDir, center, glm::vec3(0, 1, 0));

	f32 halfWidth = width / 2.0f;
	f32 halfHeight = height / 2.0f;

	// When creating the light's projection matrix we extend the depth so that
	// close, but out of view fragments of models don't get discarded by the 
	// tight clip space and create holes in shadows that the camera can see.
	glm::mat4 projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -depth * 5, depth * 5);

	m_LightViewProjection = projection * view;
}
