#pragma once
#include "renderer/Shader.h"
#include "core/Components.h"

class Material {
public:
	Material(const Shader& shader);
	void SetDiffuse(const Ref<Texture> diffuse);
	void Bind(const LocalToWorld& toWorld);
	static Material* NewStarndardMaterial();
private:
	Ref<Texture> m_Diffuse;
	Shader m_Shader;
};
