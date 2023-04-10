#include "Enviroment.h"

Enviroment* Enviroment::Instance() {
	static Enviroment instance;
	return &instance;
}

Enviroment::Enviroment() {
	constexpr uint32_t bindingPoint = 1;
	m_UniformBuffer = std::make_unique<UniformBuffer>(bindingPoint);
	m_UniformBuffer->Register("ambientStrength", sizeof(f32));
	m_UniformBuffer->Register("lightDir", sizeof(glm::vec3));
	m_UniformBuffer->Register("lightViewProjection", sizeof(glm::mat4));
	m_UniformBuffer->FinishedRegistering();
}

void Enviroment::SetLightViewProjection(glm::mat4& lightViewProjection) {
	m_UniformBuffer->SubBufferData("lightViewProjection", &lightViewProjection);
}

void Enviroment::SetLightDir(glm::vec3 lightDir) {
	m_UniformBuffer->SubBufferData("lightDir", &lightDir);
	m_LightDir = lightDir;
}

void Enviroment::SetAmbientStrength(f32 ambientStrength) {
	m_UniformBuffer->SubBufferData("ambientStrength", &ambientStrength);
	m_AmbientStrength = ambientStrength;
}

glm::vec3 Enviroment::GetLightDir() {
	return m_LightDir;
}
