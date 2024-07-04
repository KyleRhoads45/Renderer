#pragma once
#include <glm/glm.hpp>
#include "core/Base.h"
#include "CubeMap.h"
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
	
	void SetSkyBox(const Ref<CubeMap>& skybox) { m_Skybox = skybox; }
	void BindSkybox() const { m_Skybox->Bind(); }

	Ref<CubeMap> GetSkyBox() const {return m_Skybox; }
	glm::vec3 GetLightDir() const { return m_LightDir; }
private:
	Ref<CubeMap> m_Skybox;
    Scope<UniformBuffer> m_UniformBuffer;
	glm::vec3 m_LightDir;
	glm::vec3 m_AmbientColor;
	glm::vec3 m_LightColor;
	f32 m_AmbientStrength;
	f32 m_LightStrength;
};
