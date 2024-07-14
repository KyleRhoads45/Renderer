#pragma once
#include <glm/glm.hpp>
#include "core/Base.h"
#include "CubeMap.h"
#include "PcfShadowTexture.h"
#include "UniformBuffer.h"

class Enviroment {
public:
	static Enviroment* Instance();
public:
	Enviroment();
	
	void SetLightViewProjection(glm::mat4& lightViewProjection);
	void SetLightColor(glm::vec3& color);
	void SetLightDir(glm::vec3& lightDir);
	void SetLightStrength(f32 lightStrength);
	void SetAmbientColor(glm::vec3& color);
	void SetAmbientStrength(f32 ambientStrength);

	void SetShadowPcf(i32 windowSize, i32 filterSize);
	void SetShadowPcfRadius(f32 radius);
	
	void SetSkyBox(const Ref<CubeMap>& skybox) { m_Skybox = skybox; }
	void BindSkybox(const i32 textureUnit) const { m_Skybox->Bind(textureUnit); }
	void BindPcfShadow(const i32 textureUnit) const { m_PcfShadowTexture->Bind(textureUnit); }

	i32 ShadowPcfWindowSize() const { return m_PcfShadowTexture->WindowSize(); }
	i32 ShadowPcfFilterSize() const { return m_PcfShadowTexture->FilterSize(); }
	f32 ShadowPcfFilterRadius() const { return m_PcfFilterRadius; }

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
};
