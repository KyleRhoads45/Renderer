#include <glad/glad.h>
#include "renderer/ShadowMapper.h"
#include "renderer/Enviroment.h"
#include "CityMaterial.h"

CityMaterial::CityMaterial(Shader shader, std::string texture) : Material(shader) {
	m_BaseMap = Texture::Load(texture.c_str());
}

void CityMaterial::Bind(Transform* transform) {
	m_Shader.Bind();
	m_Shader.SetMat4("model", transform->Model());
	m_Shader.SetInt("useBaseMap", true);
	m_Shader.SetInt("useNormalMap", false);
	m_Shader.SetInt("useMetallicMap", false);
	m_Shader.SetInt("useRoughnessMap", false);

	glActiveTexture(GL_TEXTURE0);
	m_BaseMap->Bind();
	m_Shader.SetInt("baseMap", 0);

	glActiveTexture(GL_TEXTURE4);
	ShadowMapper::m_ShadowMap.Bind();
	m_Shader.SetInt("shadowMap", 4);

	glActiveTexture(GL_TEXTURE5);
	Enviroment::Instance()->m_Skybox->Bind();
	m_Shader.SetInt("skybox", 5);
}
