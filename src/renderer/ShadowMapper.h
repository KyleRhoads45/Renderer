#pragma once
#include "core/Components.h"
#include "DepthTexture.h"

class ShadowMapper {
public:
	static void Init(uint32_t textureSize);
	static void PerformShadowPass();

    static DepthTexture m_ShadowMap;
	static glm::mat4 m_LightViewProjection;
private:
	static void CalculateLightViewProjection();

    static uint32_t m_TextureSize;
    static uint32_t m_DepthFrameBuffer;
    static Shader m_DepthShader;
};

