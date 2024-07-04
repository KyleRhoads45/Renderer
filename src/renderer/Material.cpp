#include <glad/glad.h>
#include "renderer/ShadowMapper.h"
#include "renderer/Enviroment.h"
#include "Material.h"

Material::Material(Shader shader)
	: m_Albedo(nullptr), m_Normal(nullptr), m_Metallic(nullptr),
	  m_Shader(std::move(shader)), m_RenderOrder(RenderOrder::opaque), m_AlphaCutoff(0.0f) { }

void Material::Bind(const LocalToWorld& toWorld) {
	m_Shader.Bind();
	m_Shader.SetMat4("model", toWorld.matrix);

	BindTextureIfExists("albedoMap", m_Albedo, 0);
	BindTextureIfExists("normalMap", m_Normal, 1);
	BindTextureIfExists("metallicRoughnessMap", m_Metallic, 2);

	bool alphaClippingEnabled = m_RenderOrder == RenderOrder::cutout;
	m_Shader.SetInt("alphaClippingEnabled", alphaClippingEnabled);
	if (alphaClippingEnabled) {
		m_Shader.SetFloat("alphaCutoff", m_AlphaCutoff);
	}

	m_Shader.SetFloat("roughness", m_Roughness);
	m_Shader.SetFloat("specularStrength", m_SpecularStrength);
	m_Shader.SetFloat("metallic", m_MetallicStrength);
	
	glActiveTexture(GL_TEXTURE3);
	ShadowMapper::m_ShadowMap.Bind();
	m_Shader.SetInt("shadowMap", 3);

	glActiveTexture(GL_TEXTURE4);
	Enviroment::Instance()->BindSkybox();
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

Material* Material::NewPbrMaterial() {
	static Shader pbrShader("src/shaders/PBR.vert", "src/shaders/PBR.frag");
	return new Material(pbrShader);
}
