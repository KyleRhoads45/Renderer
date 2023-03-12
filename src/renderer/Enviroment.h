#pragma once
#include <glm/glm.hpp>
#include "CubeMap.h"
#include "UniformBuffer.h"

class Enviroment {
public:
	static void Init();
	static void SetLightViewProjection(glm::mat4& lightViewProjection);
	static CubeMap m_Skybox;
private:
    static UniformBuffer m_UniformBuffer;
};
