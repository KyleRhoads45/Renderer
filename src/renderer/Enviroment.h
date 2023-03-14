#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "CubeMap.h"
#include "UniformBuffer.h"

class Enviroment {
public:
	static Enviroment* Instance();
	Enviroment();
	void SetLightViewProjection(glm::mat4& lightViewProjection);
	void SetLightDir(glm::vec3 lightDir);
	void SetAmbientStrength(float ambientStrength);
	glm::vec3 GetLightDir();
	std::shared_ptr<CubeMap> m_Skybox;
private:
    std::unique_ptr<UniformBuffer> m_UniformBuffer;
	glm::vec3 m_LightDir;
	float m_AmbientStrength;
};
