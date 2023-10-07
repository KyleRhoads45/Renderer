#pragma once
#include "renderer/Shader.h"
#include "core/Components.h"

class Material {
public:
	Material(const Shader& shader);
	void Bind(const LocalToWorld& toWorld);
private:
	Shader m_Shader;
};
