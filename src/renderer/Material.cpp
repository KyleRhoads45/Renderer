#include <glad/glad.h>
#include "renderer/ShadowMapper.h"
#include "renderer/Enviroment.h"
#include "Material.h"

Material::Material(Shader shader)
	: m_Diffuse(nullptr), m_Normal(nullptr), m_Specular(nullptr),
	  m_Shader(std::move(shader)), m_RenderOrder(RenderOrder::opaque), m_AlphaCutoff(0.0f) { }

void Material::Bind(const LocalToWorld& toWorld) {
	m_Shader.Bind();
	m_Shader.SetMat4("model", toWorld.matrix);

	bool useDiffuse = m_Diffuse != nullptr;
	m_Shader.SetInt("useDiffuse", useDiffuse);
	if (useDiffuse) {
		glActiveTexture(GL_TEXTURE0);
		m_Diffuse->Bind();
		m_Shader.SetInt("baseMap", 0);
	}

	bool useNormal = m_Normal != nullptr;
	m_Shader.SetInt("useNormal", useNormal);
	if (useNormal) {
		glActiveTexture(GL_TEXTURE1);
		m_Normal->Bind();
		m_Shader.SetInt("normalMap", 1);
	}

	bool alphaClippingEnabled = m_RenderOrder == RenderOrder::cutout;
	m_Shader.SetInt("alphaClippingEnabled", alphaClippingEnabled);
	if (alphaClippingEnabled) {
		m_Shader.SetFloat("alphaCutoff", m_AlphaCutoff);
	}
	
	glActiveTexture(GL_TEXTURE2);
	ShadowMapper::m_ShadowMap.Bind();
	m_Shader.SetInt("shadowMap", 2);

	glActiveTexture(GL_TEXTURE3);
	Enviroment::Instance()->m_Skybox->Bind();
	m_Shader.SetInt("skybox", 3);
}

Material* Material::NewStarndardMaterial() {
	static Shader simple("src/shaders/SimpleLit.vert", "src/shaders/SimpleLit.frag");
	return new Material(simple);
}
