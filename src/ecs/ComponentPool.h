#pragma once
#include "core/Base.h"
#include "Entity.h"

class ComponentPool {
public:
	ComponentPool(const u32 compSize);

	template<typename Component>
	Component* GetComponent(const Entity entity);

	template<typename Component>
	Component* InitComponent(const Entity entity);

private:
	void* GetComponentAddress(const Entity entity) const;

	u32 m_CompSize = 0;
	Box<u8[]> m_Buffer;
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
