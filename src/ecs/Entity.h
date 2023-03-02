#pragma once
#include <stdint.h>

class Entity {
public:
	Entity();

	template<typename Component>
	Component* GetComponent();

	uint32_t id;
};

// Forward declare GetComponent from Registry
class Registry;
template<typename Component>
Component* GetComponent(const Entity& entity);

template<typename Component>
Component* Entity::GetComponent() {
	return Registry::GetComponent<Component>(*this);
}
