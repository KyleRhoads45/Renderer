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
	
	void SetAlbedoTexture(const Ref<Texture>& texture)     { m_AlbedoTexture = texture; }
	void SetNormalTexture(const Ref<Texture>& texture)     { m_NormalTexture = texture; }
	void SetMetalRoughTexture(const Ref<Texture>& texture) { m_MetalRoughTexture = texture; }
	
	void SetRenderOrder(const RenderOrder renderOrder) { m_RenderOrder = renderOrder; }
	void SetAlphaCutoff(const f32 alphaCutoff)         { m_AlphaCutoff = alphaCutoff; }
	void SetRoughness(const f32 roughness)             { m_Roughness = roughness; }
	void SetSpecularity(const f32 specularity)         { m_Specularity = specularity; }
	void SetMetallicness(const f32 metallicness)       { m_Metallicness = metallicness; }

	RenderOrder GetRenderOrder() const { return m_RenderOrder; }
	f32 GetAlphaCutoff() const         { return m_AlphaCutoff; } 
	f32 GetRoughness() const           { return m_Roughness; }
	f32 GetMetallicness() const        { return m_Metallicness; }
	f32 GetSpecularity() const         { return m_Specularity; }
	
	std::string GetAlbedoPath() const     { return m_AlbedoTexture->Path(); }
	std::string GetNormalPath() const     { return m_NormalTexture->Path(); }
	std::string GetMetalRoughPath() const { return m_MetalRoughTexture->Path(); }

	void SetFilePath(const std::string& filePath) { m_FilePath = filePath; }
	std::string GetFilePath() const { return m_FilePath; }
	
	void Bind(const LocalToWorld& toWorld);
private:
	void BindTextureIfExists(const std::string& uniformName, const Ref<Texture> texture, u32 textureUnit);
private:
	// Needed by the editor to save changes when modified
	std::string m_FilePath;

	Ref<Texture> m_AlbedoTexture;
	Ref<Texture> m_NormalTexture;
	Ref<Texture> m_MetalRoughTexture;
	
	Shader m_Shader;
	RenderOrder m_RenderOrder;
	
	f32 m_AlphaCutoff;
	f32 m_Roughness;
	f32 m_Specularity;
	f32 m_Metallicness;
};
