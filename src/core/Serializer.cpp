#include <fstream>
#include "renderer/Material.h"
#include "Serializer.h"

YAML::Emitter& operator<<(YAML::Emitter& emitter, glm::vec3 vec3) {
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq << vec3.x << vec3.y << vec3.z << YAML::EndSeq;
	return emitter;
}

YAML::Emitter& operator<<(YAML::Emitter& emitter, glm::quat quat) {
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq << quat.w << quat.x << quat.y << quat.z << YAML::EndSeq;
	return emitter;
}

void Serializer::WriteToFile(const YAML::Emitter& emitter, const std::string& file) {
	std::ofstream fout(file);
	fout << emitter.c_str();
	fout.close();
}

void Serializer::Deserialize(const YAML::Node& node, Transform& trans){
	trans.position = node["Position"].as<glm::vec3>();
	trans.rotation = node["Orientation"].as<glm::quat>();
	trans.scale = node["Scale"].as<glm::vec3>();
}

void Serializer::WriteToFile(const std::string& file) {
	WriteToFile(m_Emitter, file);
}

void Serializer::BeginMap() {
	m_Emitter << YAML::BeginMap;
}

void Serializer::BeginKeyMap(const std::string& key) {
	m_Emitter << YAML::Key << key << YAML::BeginMap;
}

void Serializer::EndMap() {
	m_Emitter << YAML::EndMap;
}

void Serializer::Serialize(const Transform& trans) {
	m_Emitter << YAML::Key << "Transform" << YAML::BeginMap;
	{
		m_Emitter << YAML::Key << "Position";
		m_Emitter << YAML::Value << trans.position;

		m_Emitter << YAML::Key << "Orientation";
		m_Emitter << YAML::Value << trans.rotation;

		m_Emitter << YAML::Key << "Scale";
		m_Emitter << YAML::Value << trans.scale;
	}
	m_Emitter << YAML::EndMap;
}

void Serializer::Serialize(const Material& material) {
	m_Emitter << YAML::BeginMap;
	m_Emitter << YAML::Key << "AlbedoTexture" << YAML::Value << material.GetAlbedoPath();
	m_Emitter << YAML::Key << "NormalTexture" << YAML::Value << material.GetNormalPath();
	m_Emitter << YAML::Key << "MetalRoughTexture" << YAML::Value << material.GetMetalRoughPath();
	m_Emitter << YAML::Key << "AlphaCutoff" << YAML::Value << material.GetAlphaCutoff();
	m_Emitter << YAML::Key << "Metallicness" << YAML::Value << material.GetMetallicness();
	m_Emitter << YAML::Key << "Roughness" << YAML::Value << material.GetRoughness();
	m_Emitter << YAML::Key << "Specularity" << YAML::Value << material.GetSpecularity();
	m_Emitter << YAML::Key << "RenderOrder" << YAML::Value << static_cast<i32>(material.GetRenderOrder());
	m_Emitter << YAML::EndMap;
}

void Serializer::Deserialize(const std::string& file, Material& material) {
	material.SetFilePath(file);

	YAML::Node materialNode = YAML::LoadFile(file);
	YAML::Node albedoTextureNode = materialNode["AlbedoTexture"];
	YAML::Node normalTextureNode = materialNode["NormalTexture"];
	YAML::Node metalRoughTextureNode = materialNode["MetalRoughTexture"];
	YAML::Node alphaCutoffNode = materialNode["AlphaCutoff"];
	YAML::Node metallicnessNode = materialNode["Metallicness"];
	YAML::Node roughnessNode = materialNode["Roughness"];
	YAML::Node specularityNode = materialNode["Specularity"];
	YAML::Node renderOrderNode = materialNode["RenderOrder"];

	material.SetRenderOrder(static_cast<RenderOrder>(renderOrderNode.as<i32>()));
	material.SetAlphaCutoff(alphaCutoffNode.as<f32>());
	material.SetMetallicness(metallicnessNode.as<f32>());
	material.SetRoughness(roughnessNode.as<f32>());
	material.SetSpecularity(specularityNode.as<f32>());

	if (!albedoTextureNode.IsNull()) {
		const auto& albedoTextureFile = albedoTextureNode.as<std::string>();
		Ref<Texture> albedo = Texture::Load(albedoTextureFile, Texture::Type::Default);
		material.SetAlbedoTexture(albedo);
	}
	
	if (!normalTextureNode.IsNull()) {
		const auto& normalTextureFile = normalTextureNode.as<std::string>();
		Ref<Texture> normal = Texture::Load(normalTextureFile, Texture::Type::NormalMap);
		material.SetNormalTexture(normal);
	}

	if (!metalRoughTextureNode.IsNull()) {
		const auto& metalRoughTextureFile = metalRoughTextureNode.as<std::string>();
		Ref<Texture> metalRough = Texture::Load(metalRoughTextureFile, Texture::Type::Default);
		material.SetMetalRoughTexture(metalRough);
	}
}
