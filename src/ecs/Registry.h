#pragma once
#include <vector>
#include <bitset>
#include <cassert>
#include "core/Base.h"
#include "ComponentPool.h"
#include "EcsParams.h"

using EntityCompMask = std::bitset<MAX_COMPONENTS>;

class Registry {
public:
	template<typename... T>
	friend class View;
	friend class ViewIterator;
	
	static Entity Create();

	template<typename Component>
	static Component* Add(Entity entity);

	template<typename Component>
	static Component* Get(Entity entity);

	template<typename Component>
	static u32 GetComponentId();

private:
	static inline u32 entityCount = 0;
	static inline Ref<Entity[]> entities = MakeRef<Entity[]>(MAX_ENTITIES);

	static inline u32 componentCounter = 0;
	static inline std::vector<ComponentPool> pools;

	static inline Ref<EntityCompMask[]> entityCompMasks = MakeBox<EntityCompMask[]>(MAX_ENTITIES);
};

template<typename Component>
Component* Registry::Add(Entity entity) {
	assert(entity.id != MAX_ENTITIES); // Unregistered entity

	u32 compId = GetComponentId<Component>();

	if (pools.size() == compId) {
		pools.push_back(ComponentPool(sizeof(Component)));
	}

	entityCompMasks[entity.id].set(compId);
	return pools[compId].InitComponent<Component>(entity);
}

template<typename Component>
Component* Registry::Get(Entity entity) {
	assert(entity.id != MAX_ENTITIES); // Unregistered entity
	u32 compId = GetComponentId<Component>();
	return pools[compId].GetComponent<Component>(entity);
}

template<typename Component>
uint32_t Registry::GetComponentId() {
	static u32 compId = componentCounter++;
	return compId;
}

