#pragma once
#include <vector>
#include <bitset>
#include "core/Base.h"
#include "ComponentPool.h"

constexpr auto MAX_COMPONENTS = 100;
using EntityCompMask = std::bitset<MAX_COMPONENTS>;

class Registry {
public:
	static Entity Create();

	template<typename Component>
	static Component& Add(Entity entity);

	template<typename Component>
	static Component& Get(Entity entity);

	template<typename Component>
	static bool Has(const Entity entity);

	template<typename Component>
	static u32 GetComponentId();

	static size_t GetEntityCount();

private:
	static inline u32 m_ComponentCounter = 0;
	static inline std::vector<Entity> m_Entities;
	static inline std::vector<ComponentPool> m_Pools;
	static inline std::vector<EntityCompMask> m_EntityCompMasks;

	template<typename... T>
	friend class View;
	friend class ViewIterator;
	friend class Editor;
	friend class Serializer;
};

template<typename Component>
Component& Registry::Add(const Entity entity) {
	const u32 compId = GetComponentId<Component>();
	
	if (m_Pools.size() == compId) {
		m_Pools.push_back(ComponentPool(sizeof(Component)));
	}

	m_EntityCompMasks[entity.Id()].set(compId);
	return m_Pools[compId].Add<Component>(entity);
}

template<typename Component>
Component& Registry::Get(const Entity entity) {
	const u32 compId = GetComponentId<Component>();
	return m_Pools[compId].Get<Component>(entity);
}


template<typename Component>
bool Registry::Has(const Entity entity) {
	const u32 compId = GetComponentId<Component>();
	return m_EntityCompMasks[entity.Id()].test(compId);
}

template<typename Component>
u32 Registry::GetComponentId() {
	static u32 compId = m_ComponentCounter++;
	return compId;
}