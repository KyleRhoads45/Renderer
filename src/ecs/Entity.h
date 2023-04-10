#pragma once
#include "EcsParams.h"
#include "core/Base.h"

class Entity {
public:
	friend class Registry;
	friend class ComponentPool;
	Entity() = default;
private:
	u32 id = MAX_ENTITIES;
};
