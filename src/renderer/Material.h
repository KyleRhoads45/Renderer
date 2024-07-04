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
	static Material* NewPbrMaterial();
public:
	explicit Material(Shader shader);
	void SetAlbedo(const Ref<Texture>& albedo)     { m_Albedo = albedo; }
	void SetNormal(const Ref<Texture>& normal)     { m_Normal = normal; }
	void SetMetallic(const Ref<Texture>& metallic) { m_Metallic = metallic; }
	
	void SetRenderOrder(const RenderOrder renderOrder)   { m_RenderOrder = renderOrder; }
	void SetAlphaCutoff(const f32 alphaCutoff)           { m_AlphaCutoff = alphaCutoff; }
	void SetRoughness(const f32 roughness)               { m_Roughness = roughness; }
	void SetSpecularStrength(const f32 specularStrength) { m_SpecularStrength = specularStrength; }
	void SetMetallicStrength(const f32 metallicStrength) { m_MetallicStrength = metallicStrength; }
	
	RenderOrder GetRenderOrder() const { return m_RenderOrder; }
	
	void Bind(const LocalToWorld& toWorld);
	std::string DiffuseTexturePath() const { return m_Albedo->Path(); }
private:
	void BindTextureIfExists(const std::string& uniformName, const Ref<Texture>& texture, u32 textureUnit);
private:
	Ref<Texture> m_Albedo;
	Ref<Texture> m_Normal;
	Ref<Texture> m_Metallic;
	Shader m_Shader;
	RenderOrder m_RenderOrder;
	f32 m_AlphaCutoff;
	f32 m_Roughness;
	f32 m_SpecularStrength;
	f32 m_MetallicStrength;
};
