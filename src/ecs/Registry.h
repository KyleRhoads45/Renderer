#pragma once
#include <vector>
#include <bitset>
#include "core/Base.h"
#include "ComponentPool.h"

constexpr auto MAX_COMPONENTS = 100;

class EntityCompMask;

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

	//template<typename... Components>
	//static EntityCompMask CreateComponentMask();

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

class EntityCompMask {
public:

	EntityCompMask() = default;

	template<typename... Components>
	static EntityCompMask From() {
		EntityCompMask mask;
		u32 componentIds[] = {Registry::GetComponentId<Components>()...};
		for (int i = 0; i < sizeof...(Components); i++) {
			mask.Set(componentIds[i]);
		}
		return mask;
	}

	inline bool operator==(const EntityCompMask& otherMask) const {
		return m_Mask == otherMask.m_Mask;
	}

	inline bool Empty() {
		return m_Mask.none();
	}

	inline bool IsSubsetOf(const EntityCompMask& superSet) const {
		return (m_Mask & superSet.m_Mask) == m_Mask;
	}

	inline bool SharesAnyWith(const EntityCompMask& otherMask) const {
		return (m_Mask & otherMask.m_Mask).any();
	}

	inline void Set(u32 index) {
		m_Mask.set(index);
	}

	inline bool Test(u32 index) {
		return m_Mask.test(index);
	}

public:
	std::bitset<MAX_COMPONENTS> m_Mask;
};

template<typename Component>
Component& Registry::Add(const Entity entity) {
	const u32 compId = GetComponentId<Component>();
	
	if (m_Pools.size() == compId) {
		m_Pools.push_back(ComponentPool(sizeof(Component)));
	}

	m_EntityCompMasks[entity.Id()].Set(compId);
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
	return m_EntityCompMasks[entity.Id()].Test(compId);
}

template<typename Component>
u32 Registry::GetComponentId() {
	static u32 compId = m_ComponentCounter++;
	return compId;
}

//template<typename... Components>
//EntityCompMask Registry::CreateComponentMask() {
//	EntityCompMask mask;
//
//	u32 componentIds[] = {Registry::GetComponentId<Components>()...};
//	for (int i = 0; i < sizeof...(Components); i++) {
//		mask.set(componentIds[i]);
//	}
//
//	return mask;
//}
