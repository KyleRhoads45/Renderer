#include "Enviroment.h"

CubeMap Enviroment::m_Skybox;
UniformBuffer Enviroment::m_UniformBuffer;
glm::vec3 Enviroment::m_LightDir;
float Enviroment::m_AmbientStrength;

void Enviroment::Init() {
	constexpr uint32_t bindingPoint = 1;
	m_UniformBuffer = UniformBuffer(bindingPoint);
	m_UniformBuffer.Register("ambientStrength", sizeof(float));
	m_UniformBuffer.Register("lightDir", sizeof(glm::vec3));
	m_UniformBuffer.Register("lightViewProjection", sizeof(glm::mat4));
	m_UniformBuffer.FinishedRegistering();
}

void Enviroment::SetLightViewProjection(glm::mat4& lightViewProjection) {
	m_UniformBuffer.SubBufferData("lightViewProjection", &lightViewProjection);
}

void Enviroment::SetLightDir(glm::vec3 lightDir) {
	m_UniformBuffer.SubBufferData("lightDir", &lightDir);
	m_LightDir = lightDir;
}

void Enviroment::SetAmbientStrength(float ambientStrength) {
	m_UniformBuffer.SubBufferData("ambientStrength", &ambientStrength);
	m_AmbientStrength = ambientStrength;
}

glm::vec3 Enviroment::GetLightDir() {
	return m_LightDir;
}
