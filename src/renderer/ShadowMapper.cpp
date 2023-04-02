#include <glad/glad.h>
#include "Model.h"
#include "ecs/Registry.h"
#include "ecs/View.h"
#include "Renderer.h"
#include "core/Primatives.h"
#include "core/CameraSystem.h"
#include "Enviroment.h"
#include "ShadowMapper.h"

void ShadowMapper::Init(uint32_t textureSize, uint32_t shadowDist) {
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

	int32_t viewportDimensions[4];
	glGetIntegerv(GL_VIEWPORT, viewportDimensions);
	int32_t viewportWidth = viewportDimensions[2];
	int32_t viewportHeight = viewportDimensions[3];

	glViewport(0, 0, m_TextureSize, m_TextureSize);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthFrameBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	m_DepthShader.Bind();
	m_DepthShader.SetMat4("viewProjection", m_LightViewProjection);

    for (auto entity : View<Transform, MeshRenderer>()) {
        auto transform = entity.Get<Transform>();
        auto meshRenderer = entity.Get<MeshRenderer>();
        
		m_DepthShader.SetMat4("model", transform->Model());

        glBindVertexArray(meshRenderer->mesh.vao);
        glDrawElements(GL_TRIANGLES, meshRenderer->mesh.numIndices, GL_UNSIGNED_INT, 0);
    }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, viewportWidth, viewportHeight);

	Enviroment::Instance()->SetLightViewProjection(m_LightViewProjection);
}

void ShadowMapper::CalculateLightViewProjection() {
	auto frustrumPoints = CameraSystem::GetViewFrustrumPoints(m_ShadowDist);

	glm::vec3 lightDir = Enviroment::Instance()->GetLightDir();
	glm::mat4 lightSpaceView = glm::lookAt(glm::vec3(0, 0, 0), lightDir, glm::vec3(0, 1, 0));

	// Convert camera frustrum points to light space
	// its as if we are viewing the frustrum from the lights point of view.
	// Note: this is still in world space so its not always centered at origin. 
	for (int i = 0; i < frustrumPoints.size(); i++) {
		frustrumPoints[i] = lightSpaceView * glm::vec4(frustrumPoints[i], 1.0f);
	}

	Bounds bounds(frustrumPoints.data(), frustrumPoints.size());

	float width = glm::abs(bounds.max.x - bounds.min.x);
	float height = glm::abs(bounds.max.y - bounds.min.y);
	float depth = glm::abs(bounds.max.z - bounds.min.z);

	// Transform the min and max points from light space into world space.
	// Calculate the center of the bounding box in world space. 
	glm::vec3 worldSpaceMin = glm::inverse(lightSpaceView) * glm::vec4(bounds.min, 1.0f);
	glm::vec3 worldSpaceMax = glm::inverse(lightSpaceView) * glm::vec4(bounds.max, 1.0f);
	glm::vec3 center = (worldSpaceMin + worldSpaceMax) / 2.0f;

	// Create the light's view matrix with the view position being
	// the center of the bounding box.
	glm::mat4 view = glm::lookAt(center - lightDir, center, glm::vec3(0, 1, 0));

	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;

	// When creating the light's projection matrix we extend the depth so that
	// close, but out of view fragments of models don't get discarded by the 
	// tight clip space and create holes in shadows that the camera can see.
	glm::mat4 projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -depth, depth);

	m_LightViewProjection = projection * view;
}
