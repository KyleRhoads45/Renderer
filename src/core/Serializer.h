#pragma once
#include <yaml-cpp/yaml.h>
#include "Components.h"

class Material;

class Serializer {
public:
	static void WriteToFile(const YAML::Emitter& emitter, const std::string& file);
	static void Deserialize(const YAML::Node& node, Transform& trans);
	static void Deserialize(const std::string& file, Material& material);
public:
	Serializer() = default;

	void WriteToFile(const std::string& file);

	void BeginMap();
	void BeginKeyMap(const std::string& key);
	void EndMap();

	template<typename T> 
	void WriteKeyValue(const std::string& key, T& value);

	template<typename T> 
	void WriteKeyValue(const std::string& key, T&& value);

	void Serialize(const Transform& trans);
	void Serialize(const Material& material);
private:
	YAML::Emitter m_Emitter;
};

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, glm::vec2 vec2) {
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq << vec2.x << vec2.y << YAML::EndSeq;
	return emitter;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, glm::vec3 vec3) {
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq << vec3.x << vec3.y << vec3.z << YAML::EndSeq;
	return emitter;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, glm::quat quat) {
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq << quat.w << quat.x << quat.y << quat.z << YAML::EndSeq;
	return emitter;
}

template<typename T> 
void Serializer::WriteKeyValue(const std::string& key, T& value) {
	m_Emitter << YAML::Key << key << YAML::Value << value;
}

template<typename T>
void Serializer::WriteKeyValue(const std::string& key, T&& value) {
	m_Emitter << YAML::Key << key << YAML::Value << value;
}

namespace YAML {

	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2 rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			rhs.x = node[0].as<f32>();
			rhs.y = node[1].as<f32>();
			return true;
		}
	};
	
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