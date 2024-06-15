#pragma once
#include <yaml-cpp/yaml.h>
#include "ecs/Registry.h"
#include "Components.h"

class Material;

struct MeshRendererImport {
	std::vector<i32> meshIndicies;
	std::vector<std::string> materialFilePaths;
};

class Serializer {
public:
	Serializer() = default;

	static void SerializeScene();
	static void SerializeEntity(YAML::Emitter& out, Entity entity);
	static void SerializeMaterial(const std::string& file, const Material& material);
	static void WriteToFile(YAML::Emitter& emitter, const std::string& file);

	i32 BeginEntity(const i32 parentId);
	void EndEntity();
	void WriteToFile(const std::string& file);

	void BeginMap();
	void EndMap();

	template<typename T> 
	void WriteKeyValue(const std::string& key, T& value);

	void Serialize(const Transform& trans);
	void Serialize(const MeshRendererImport& meshRendrImport);
	void Serialize(const Material& material);

	void Deserialize();
private:
	YAML::Emitter m_Emitter;
};

template<typename T> 
void Serializer::WriteKeyValue(const std::string& key, T& value) {
	m_Emitter << YAML::Key << key << YAML::Value << value;
}

namespace YAML {
	
	template<>
	struct convert<glm::vec3> {
		
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			rhs.x = node[0].as<f32>();
			rhs.y = node[1].as<f32>();
			rhs.z = node[2].as<f32>();
			return true;
		}
	};
	
	template<>
	struct convert<glm::quat> {
		
		static Node encode(const glm::quat& rhs) {
			Node node;
			node.push_back(rhs.w);
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::quat& rhs) {
			rhs.w = node[0].as<f32>();
			rhs.x = node[1].as<f32>();
			rhs.y = node[2].as<f32>();
			rhs.z = node[3].as<f32>();
			return true;
		}
	};
	
}