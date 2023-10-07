#include <glad/glad.h>
#include "renderer/ShadowMapper.h"
#include "renderer/Enviroment.h"
#include "Material.h"

Material::Material(const Shader& shader) {
	m_Shader = shader;
}

void Material::Bind(const LocalToWorld& toWorld) {
	m_Shader.Bind();
	m_Shader.SetMat4("model", toWorld.matrix);

	glActiveTexture(GL_TEXTURE4);
	ShadowMapper::m_ShadowMap.Bind();
	m_Shader.SetInt("shadowMap", 0);

	glActiveTexture(GL_TEXTURE5);
	Enviroment::Instance()->m_Skybox->Bind();
	m_Shader.SetInt("skybox", 1);
}