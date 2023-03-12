#include "Enviroment.h"

CubeMap Enviroment::m_Skybox;
UniformBuffer Enviroment::m_UniformBuffer;

void Enviroment::Init() {
	constexpr uint32_t bindingPoint = 1;
	m_UniformBuffer = UniformBuffer(bindingPoint);
	m_UniformBuffer.Register("lightViewProjection", sizeof(glm::mat4));
	m_UniformBuffer.FinishedRegistering();
}

void Enviroment::SetLightViewProjection(glm::mat4& lightViewProjection) {
	m_UniformBuffer.SubBufferData("lightViewProjection", &lightViewProjection);
}
