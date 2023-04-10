#include "Material.h"

Material::Material(Shader shader) {
	m_Shader = shader;
}

void Material::Bind() { }

void Material::Bind(Transform* transform) { }
