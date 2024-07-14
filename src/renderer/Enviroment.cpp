#include "Enviroment.h"

Enviroment* Enviroment::Instance() {
	static Enviroment instance;
	return &instance;
}

Enviroment::Enviroment() {
	constexpr u32 bindingPoint = 1;
	m_UniformBuffer = std::make_unique<UniformBuffer>(bindingPoint);
	m_UniformBuffer->Register("lightColor", sizeof(glm::vec3));
	m_UniformBuffer->Register("ambientColor", sizeof(glm::vec3));
	m_UniformBuffer->Register("ambientStrength", sizeof(f32));
	m_UniformBuffer->Register("lightStrength", sizeof(f32));
	m_UniformBuffer->Register("lightDir", sizeof(glm::vec3));
	m_UniformBuffer->Register("lightViewProjection", sizeof(glm::mat4));
	m_UniformBuffer->Register("pcfWindowSize", sizeof(i32));
	m_UniformBuffer->Register("pcfFilterSize", sizeof(i32));
	m_UniformBuffer->Register("pcfFilterRadius", sizeof(f32));
	m_UniformBuffer->FinishedRegistering();

	SetShadowPcf(16, 10);
	SetShadowPcfRadius(5.0f);
}

void Enviroment::SetLightViewProjection(glm::mat4& lightViewProjection) {
	m_UniformBuffer->SubBufferData("lightViewProjection", &lightViewProjection);
}

void Enviroment::SetLightColor(glm::vec3& color) {
	m_UniformBuffer->SubBufferData("lightColor", &color);
	m_LightColor = color;
}

void Enviroment::SetLightDir(glm::vec3& lightDir) {
	m_UniformBuffer->SubBufferData("lightDir", &lightDir);
	m_LightDir = lightDir;
}

void Enviroment::SetLightStrength(f32 lightStrength) {
	m_UniformBuffer->SubBufferData("lightStrength", &lightStrength);
	m_LightStrength = lightStrength;
}

void Enviroment::SetAmbientColor(glm::vec3& color) {
	m_UniformBuffer->SubBufferData("ambientColor", &color);
	m_AmbientColor = color;
}

void Enviroment::SetAmbientStrength(f32 ambientStrength) {
	m_UniformBuffer->SubBufferData("ambientStrength", &ambientStrength);
	m_AmbientStrength = ambientStrength;
}

void Enviroment::SetShadowPcf(i32 windowSize, i32 filterSize) {
	m_PcfShadowTexture = MakeRef<PcfShadowTexture>(windowSize, filterSize);
	
	i32 pcfWindowSize = m_PcfShadowTexture->WindowSize();
	i32 pcfFilterSize = m_PcfShadowTexture->FilterSize();
	
	m_UniformBuffer->SubBufferData("pcfWindowSize", &pcfWindowSize);
	m_UniformBuffer->SubBufferData("pcfFilterSize", &pcfFilterSize);
}

void Enviroment::SetShadowPcfRadius(f32 radius) {
	m_UniformBuffer->SubBufferData("pcfFilterRadius", &radius);
	m_PcfFilterRadius = radius;
}
