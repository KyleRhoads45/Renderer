#pragma once
#include <glm/glm.hpp>

#include "Bounds.h"
#include "Bounds.h"
#include "core/Base.h"
#include "CubeMap.h"
#include "PcfShadowTexture.h"
#include "UniformBuffer.h"

class Enviroment {
public:
	static Enviroment* Instance();
public:
	Enviroment();
	
	void SetLightViewProjection(const glm::mat4& lightViewProjection);
	void SetLightColor(const glm::vec3& color);
	void SetLightDir(const glm::vec3& lightDir);
	void SetLightStrength(f32 lightStrength);
	void SetAmbientColor(const glm::vec3& color);
	void SetAmbientStrength(f32 ambientStrength);

	void SetShadowPcf(i32 windowSize, i32 filterSize);
	void SetShadowPcfRadius(f32 radius);
	void SetShadowStrength(f32 shadowStrength);
	
	void SetSkyBox(const Ref<CubeMap>& skybox) { m_Skybox = skybox; }
	void BindSkybox(const i32 textureUnit) const { m_Skybox->Bind(textureUnit); }
	void BindPcfShadow(const i32 textureUnit) const { m_PcfShadowTexture->Bind(textureUnit); }

	i32 ShadowPcfWindowSize() const { return m_PcfShadowTexture->WindowSize(); }
	i32 ShadowPcfFilterSize() const { return m_PcfShadowTexture->FilterSize(); }
	f32 ShadowPcfFilterRadius() const { return m_PcfFilterRadius; }
	f32 ShadowStrength() const { return m_ShadowStrength; }

	f32 LightStrength() const { return m_LightStrength; }
	f32 AmbientStrength() const { return m_AmbientStrength; }
	glm::vec3 LightColor() const { return m_LightColor; }
	glm::vec3 AmbientColor() const { return m_AmbientColor; }

	Ref<CubeMap> GetSkyBox() const { return m_Skybox; }
	glm::vec3 GetLightDir() const { return m_LightDir; }
private:
	Ref<CubeMap> m_Skybox;
    Scope<UniformBuffer> m_UniformBuffer;
    Ref<PcfShadowTexture> m_PcfShadowTexture;
	glm::vec3 m_LightDir;
	glm::vec3 m_AmbientColor;
	glm::vec3 m_LightColor;
	f32 m_AmbientStrength;
	f32 m_LightStrength;
	f32 m_PcfFilterRadius;
	f32 m_ShadowStrength;
};
