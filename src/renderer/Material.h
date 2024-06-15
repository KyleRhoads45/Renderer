#pragma once
#include <string>
#include <bitset>
#include "Texture.h"
#include "renderer/Shader.h"
#include "core/Components.h"

enum class RenderOrder {
	opaque,
	cutout,
	transparent,
};

class Material {
public:
	static Material* NewStarndardMaterial();
public:
	explicit Material(Shader shader);
	void SetDiffuse(const Ref<Texture>& diffuse) { m_Diffuse = diffuse; }
	void SetNormal(const Ref<Texture>& normal)   { m_Normal = normal; }
	
	void SetRenderOrder(const RenderOrder renderOrder) { m_RenderOrder = renderOrder; }
	void SetAlphaCutoff(const f32 alphaCutoff) { m_AlphaCutoff = alphaCutoff; }
	RenderOrder GetRenderOrder() const { return m_RenderOrder; }
	
	void Bind(const LocalToWorld& toWorld);
	std::string DiffuseTexturePath() const { return m_Diffuse->Path(); }
private:
	Ref<Texture> m_Diffuse;
	Ref<Texture> m_Normal;
	Ref<Texture> m_Specular;
	Shader m_Shader;
	RenderOrder m_RenderOrder;
	f32 m_AlphaCutoff;
};
