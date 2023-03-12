#pragma once
#include <stdint.h>
#include <cassert>
#include "EcsParams.h"

class Entity {
public:
	friend class Registry;
	friend class ComponentPool;

	Entity() = default;

	template<typename Component>
	Component* Get();
private:
	uint32_t id = MAX_ENTITIES;
};

// Forward declare GetComponent from Registry
class Registry;
template<typename Component>
Component* Get(Entity entity);

template<typename Component>
Component* Entity::Get() {
	// Don't call GetComponent on unregistered entity
	assert(id != MAX_ENTITIES);
	return Registry::Get<Component>(*this);
}
