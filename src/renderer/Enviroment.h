#pragma once
#include <glm/glm.hpp>
#include "CubeMap.h"
#include "UniformBuffer.h"

class Enviroment {
public:
	static void Init();
	static void SetLightViewProjection(glm::mat4& lightViewProjection);
	static void SetLightDir(glm::vec3 lightDir);
	static void SetAmbientStrength(float ambientStrength);
	static glm::vec3 GetLightDir();
	static CubeMap m_Skybox;
private:
    static UniformBuffer m_UniformBuffer;
	static glm::vec3 m_LightDir;
	static float m_AmbientStrength;
};
