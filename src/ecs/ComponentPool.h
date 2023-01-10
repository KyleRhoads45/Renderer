#pragma once
#include <memory>
#include "Entity.h"

class ComponentPool {
public:
	ComponentPool(const size_t componentSize);

	template<typename Component>
	Component* GetComponent(const Entity entity);

	template<typename Component>
	Component* InitComponent(const Entity entity);

private:
	void* GetComponentAddress(const Entity entity) const;

	size_t componentSize = 0;
	std::unique_ptr<std::byte[]> buffer;
};

template <typename Component>
Component* ComponentPool::GetComponent(const Entity entity) {
	return (Component*)GetComponentAddress(entity);
}

template <typename Component>
Component* ComponentPool::InitComponent(const Entity entity) {
	Component* component = new (GetComponentAddress(entity))Component;
	return component;
}
