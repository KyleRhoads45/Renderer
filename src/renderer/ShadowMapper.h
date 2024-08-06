#pragma once
#include "core/Base.h"
#include "DepthTexture.h"
#include "Shader.h"
#include <glm/glm.hpp>

class ShadowMapper {
public:
	static void Init(const u32 textureSize, const f32 shadowDist);
	static void PerformShadowPass();
	static void BindShadowMap(const i32 textureUnit);
private:
	static void CalculateLightViewProjection();
private:
    inline static DepthTexture m_ShadowMap;
    inline static Shader m_DepthShader;
	inline static glm::mat4 m_LightViewProjection;
    inline static u32 m_TextureSize;
    inline static u32 m_DepthFrameBuffer;
    inline static f32 m_ShadowDist;
};

