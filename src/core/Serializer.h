#pragma once
#include <yaml-cpp/yaml.h>
#include "ecs/Registry.h"

class Serializer {
public:
	static void SerializeScene();
	static void SerializeEntity(YAML::Emitter& out, Entity entity);
};

