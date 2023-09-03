#include <fstream>
#include "Components.h"
#include "Serializer.h"

YAML::Emitter& operator<<(YAML::Emitter& emitter, glm::vec3 vec3) {
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq << vec3.x << vec3.y << vec3.z << YAML::EndSeq;
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
