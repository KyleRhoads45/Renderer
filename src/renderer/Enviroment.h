#pragma once
#include <glm/glm.hpp>
#include "core/Base.h"
#include "CubeMap.h"
#include "UniformBuffer.h"

class Enviroment {
public:
	static Enviroment* Instance();
	Enviroment();
	void SetLightViewProjection(glm::mat4& lightViewProjection);
	void SetLightDir(glm::vec3 lightDir);
	void SetLightStrength(f32 lightStrength);
	void SetAmbientStrength(f32 ambientStrength);
	glm::vec3 GetLightDir();
	Ref<CubeMap> m_Skybox;
private:
    Scope<UniformBuffer> m_UniformBuffer;
	glm::vec3 m_LightDir;
	f32 m_AmbientStrength;
	f32 m_LightStrength;
};
