#include <glad/glad.h>
#include "renderer/ShadowMapper.h"
#include "renderer/Enviroment.h"
#include "Material.h"

Material::Material(const Shader& shader) 
	:m_Shader(shader) { }

void Material::Bind(const LocalToWorld& toWorld) {
	m_Shader.Bind();
	m_Shader.SetMat4("model", toWorld.matrix);

	glActiveTexture(GL_TEXTURE0);
	m_Diffuse->Bind();
	m_Shader.SetInt("baseMap", 0);

	glActiveTexture(GL_TEXTURE1);
	ShadowMapper::m_ShadowMap.Bind();
	m_Shader.SetInt("shadowMap", 1);

	glActiveTexture(GL_TEXTURE2);
	Enviroment::Instance()->m_Skybox->Bind();
	m_Shader.SetInt("skybox", 2);
}

void Material::SetDiffuse(const Ref<Texture> diffuse) {
	m_Diffuse = diffuse;
}

Material* Material::NewStarndardMaterial() {
	static Shader simple("src/shaders/SimpleLit.vert", "src/shaders/SimpleLit.frag");
	return new Material(simple);
}
