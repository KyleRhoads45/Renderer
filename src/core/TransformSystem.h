#pragma once
#include "ecs/Registry.h"

class TransformSystem {
public:
	static void Update(Registry& registry);
private:
	static void UpdateLocalToWorld(Registry& registry, const Entity entity);
};

