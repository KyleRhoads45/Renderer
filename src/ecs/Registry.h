#pragma once
#include <vector>
#include <bitset>
#include "core/Base.h"
#include "ComponentPool.h"

constexpr auto MAX_COMPONENTS = 100;

class Entity;
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

class Entity {
public:
	Entity() = default;
	Entity(const size_t id);

	bool operator==(const Entity& other) const;

	size_t Id() const;

	template<typename Component>
	Component& Get();

	template<typename Component>
	Component& Add();

	template<typename Component>
	Component& GetAdd();

	template<typename Component>
	bool Has();

	static Entity Null();
private:
	size_t m_Id;
};

class EntityCompMask {
public:
	EntityCompMask() = default;

	template<typename... Components>
	static EntityCompMask From();

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
	
	if (m_Pools.size() == compId) {
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
u32 Registry::GetComponentId() {
	static u32 compId = m_ComponentCounter++;
	return compId;
}

template<typename Component>
Component& Entity::Get() {
	return Registry::Get<Component>(m_Id);
}

template<typename Component>
Component& Entity::Add() {
	return Registry::Add<Component>(m_Id);
}

template<typename Component>
Component& Entity::GetAdd() {
	if (Has<Component>()) {
		return Get<Component>();
	}
	return Add<Component>();
}

template<typename Component>
bool Entity::Has() {
	return Registry::Has<Component>(m_Id);
}

template<typename... Components>
EntityCompMask EntityCompMask::From() {
	EntityCompMask mask;
	u32 componentIds[] = { Registry::GetComponentId<Components>()... };
	for (int i = 0; i < sizeof...(Components); i++) {
		mask.Set(componentIds[i]);
	}
	return mask;
}

