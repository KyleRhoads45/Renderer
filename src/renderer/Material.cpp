#include <glad/glad.h>
#include "renderer/ShadowMapper.h"
#include "renderer/Enviroment.h"
#include "Material.h"

Material::Material(Shader shader)
	: m_Albedo(nullptr), m_Normal(nullptr), m_Specular(nullptr),
	  m_Shader(std::move(shader)), m_RenderOrder(RenderOrder::opaque), m_AlphaCutoff(0.0f) { }

void Material::Bind(const LocalToWorld& toWorld) {
	m_Shader.Bind();
	m_Shader.SetMat4("model", toWorld.matrix);

	BindTextureIfExists("albedoMap", m_Albedo, 0);
	BindTextureIfExists("normalMap", m_Normal, 1);
	BindTextureIfExists("specularMap", m_Specular, 2);

	bool alphaClippingEnabled = m_RenderOrder == RenderOrder::cutout;
	m_Shader.SetInt("alphaClippingEnabled", alphaClippingEnabled);
	if (alphaClippingEnabled) {
		m_Shader.SetFloat("alphaCutoff", m_AlphaCutoff);
	}
	
	glActiveTexture(GL_TEXTURE3);
	ShadowMapper::m_ShadowMap.Bind();
	m_Shader.SetInt("shadowMap", 3);

	glActiveTexture(GL_TEXTURE4);
	Enviroment::Instance()->m_Skybox->Bind();
	m_Shader.SetInt("skybox", 4);
}

void Material::BindTextureIfExists(const std::string& uniformName, const Ref<Texture>& texture, const u32 textureUnit) {
	bool useTexture = texture != nullptr;
	m_Shader.SetInt(uniformName + "Enabled", useTexture);
	if (useTexture) {
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		texture->Bind();
		m_Shader.SetInt(uniformName, textureUnit);
	}
}

Material* Material::NewStarndardMaterial() {
	static Shader simple("src/shaders/SimpleLit.vert", "src/shaders/SimpleLit.frag");
	return new Material(simple);
}
