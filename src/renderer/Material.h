#pragma once
#include "renderer/Shader.h"
#include "core/Components.h"

class Material {
public:
	Material(const Shader& shader);
	void Bind(const Transform& transform);
private:
	Shader m_Shader;
};
