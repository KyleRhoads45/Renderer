#pragma once
#include <vector>
#include <bitset>
#include <filesystem>
#include <iostream>

#include "core/Base.h"
#include "ComponentPool.h"

constexpr auto MAX_COMPONENTS = 100;

class Entity;
class EntityCompMask;

class Registry {
public:
	Entity Create();
	void FreeAll();

	template<typename Component>
	Component& Add(Entity entity);

	template<typename Component>
	Component& Get(Entity entity);

	template<typename Component>
	bool Has(const Entity entity);
	
	template<typename Component>
	Component& GetAdd(Entity entity);

	template<typename Component>
	u32 GetComponentId();

	size_t GetEntityCount();
private:
	inline static u32 m_ComponentCounter = 0;
	
	std::vector<Entity> m_Entities;
	std::vector<ComponentPool> m_Pools;
	std::vector<EntityCompMask> m_EntityCompMasks;

	template<typename... T>
	friend class View;
	friend class ViewIterator;
	friend class Editor;
	friend class Serializer;
	friend class Selection;
};

class Entity {
public:
	Entity() = default;
	Entity(const size_t id);
	bool operator==(const Entity& other) const;
	size_t Id() const;
	static Entity Null();
private:
	size_t m_Id = -1;
};

class EntityCompMask {
public:
	EntityCompMask() = default;

	template<typename... Components>
	static EntityCompMask From(Registry& registry);

	bool operator==(const EntityCompMask& otherMask) const;

	bool Empty() const;
	bool IsSubsetOf(const EntityCompMask& superSet) const;
	bool SharesAnyWith(const EntityCompMask& otherMask) const;
	void Set(u32 index);
	bool Test(u32 index) const;
public:
	std::bitset<MAX_COMPONENTS> m_Mask;
};

template<typename Component>
Component& Registry::Add(const Entity entity) {
	const u32 compId = GetComponentId<Component>();

	for (int i = m_Pools.capacity(); i <= compId; i++) {
		m_Pools.push_back(ComponentPool(sizeof(Component)));
	}

	m_EntityCompMasks[entity.Id()].Set(compId);
	return m_Pools[compId].Add<Component>(entity.Id());
}

template<typename Component>
Component& Registry::Get(const Entity entity) {
	const u32 compId = GetComponentId<Component>();
	return m_Pools[compId].Get<Component>(entity.Id());
}

template<typename Component>
bool Registry::Has(const Entity entity) {
	const u32 compId = GetComponentId<Component>();
	return m_EntityCompMasks[entity.Id()].Test(compId);
}

template<typename Component>
Component& Registry::GetAdd(const Entity entity) {
	if (Has<Component>(entity)) {
		return Get<Component>(entity);
	}
	return Add<Component>(entity);
}

template<typename Component>
u32 Registry::GetComponentId() {
	static u32 compId = m_ComponentCounter++;
	return compId;
}

template<typename... Components>
EntityCompMask EntityCompMask::From(Registry& registry) {
	EntityCompMask mask;
	u32 componentIds[] = { registry.GetComponentId<Components>()... };
	for (int i = 0; i < sizeof...(Components); i++) {
		mask.Set(componentIds[i]);
	}
	return mask;
}