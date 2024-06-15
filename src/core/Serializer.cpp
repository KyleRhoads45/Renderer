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

void Serializer::SerializeScene() {
	YAML::Emitter out;
	for (Entity entity : Registry::m_Entities) {
		SerializeEntity(out, entity);
	}

	std::ofstream fout("SampleScene.yaml");
	fout << out.c_str();
	fout.close();
}

void Serializer::SerializeEntity(YAML::Emitter& out, Entity entity) {
	out << YAML::BeginMap;

	out << YAML::Key << "Entity";
	out << YAML::Value << entity.Id();

	if (Registry::Has<Transform>(entity)) {
		out << YAML::Key << "Transform";

		auto& trans = Registry::Get<Transform>(entity);

		out << YAML::BeginMap;
		out << YAML::Key << "Position";
		out << YAML::Value << trans.position;
		out << YAML::EndMap;
	}

	out << YAML::EndMap;
}

void Serializer::SerializeMaterial(const std::string& file, const Material& material) {
	Serializer serializer;
	serializer.Serialize(material);
	serializer.WriteToFile(file);
}

void Serializer::WriteToFile(YAML::Emitter& emitter, const std::string& file) {
	std::ofstream fout(file);
	fout << emitter.c_str();
	fout.close();
}

i32 Serializer::BeginEntity(const i32 parentId) {
	static i32 entityId = -1;
	entityId++;
	m_Emitter << YAML::BeginMap;
	m_Emitter << YAML::Key << "EntityId" << YAML::Value << entityId;

	if (parentId >= 0) {
		m_Emitter << YAML::Key << "ParentId" << YAML::Value << parentId;
	}
	else {
		m_Emitter << YAML::Key << "ParentId" << YAML::Value << YAML::Null;
	}

	m_Emitter << YAML::Key << "Components";
	m_Emitter << YAML::BeginMap;
	return entityId;
}

void Serializer::EndEntity() {
	m_Emitter << YAML::EndMap;
	m_Emitter << YAML::EndMap;
}

void Serializer::WriteToFile(const std::string& file) {
	Serializer::WriteToFile(m_Emitter, file);
}

void Serializer::BeginMap() {
	m_Emitter << YAML::BeginMap;
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

void Serializer::Serialize(const MeshRendererImport& meshRendrImport) {
	m_Emitter << YAML::Key << "MeshRenderer" << YAML::BeginMap;
	{
		m_Emitter << YAML::Key << "MeshIndicies" << YAML::Value << meshRendrImport.meshIndicies;
		m_Emitter << YAML::Key << "Materials" << YAML::Value << meshRendrImport.materialFilePaths;
	}
	m_Emitter << YAML::EndMap;
}

void Serializer::Serialize(const Material& material) {
	m_Emitter << YAML::Key << "DiffuseTexture" << YAML::Value << material.DiffuseTexturePath();
}
