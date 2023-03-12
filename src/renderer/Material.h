#pragma once
#include "renderer/Shader.h"
#include "core/Components/Transform.h"

class Material {
public:
	Material(Shader shader);
	virtual void Bind();
	virtual void Bind(Transform* transform);
protected:
	Shader m_Shader;
};
