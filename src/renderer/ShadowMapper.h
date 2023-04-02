#pragma once
#include "core/Components.h"
#include "DepthTexture.h"

class ShadowMapper {
public:
	static void Init(uint32_t textureSize, uint32_t shadowDist);
	static void PerformShadowPass();

    inline static DepthTexture m_ShadowMap;
	inline static glm::mat4 m_LightViewProjection;
private:
	static void CalculateLightViewProjection();

    inline static uint32_t m_TextureSize;
    inline static uint32_t m_ShadowDist;
    inline static uint32_t m_DepthFrameBuffer;
    inline static Shader m_DepthShader;
};

