#pragma once
#include "core/Base.h"
#include "DepthTexture.h"
#include "Shader.h"
#include <glm/glm.hpp>

class ShadowMapper {
public:
	static void Init(u32 textureSize, u32 shadowDist);
	static void PerformShadowPass();
public:
    inline static DepthTexture m_ShadowMap;
	inline static glm::mat4 m_LightViewProjection;
private:
	static void CalculateLightViewProjection();
private:
    inline static u32 m_TextureSize;
    inline static u32 m_ShadowDist;
    inline static u32 m_DepthFrameBuffer;
    inline static Shader m_DepthShader;
};

