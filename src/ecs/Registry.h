#pragma once
#include <vector>
#include <memory>
#include <bitset>
#include "ComponentPool.h"
#include "EcsParams.h"

typedef std::bitset<MAX_COMPONENTS> EntityComponentMask;

class Registry {
public:
	template<typename... T>
	friend class View;
	friend class ViewIterator;
	
	static Entity Create();

	template<typename Component>
	static Component* AddComponent(Entity entity);

	template<typename Component>
	static Component* GetComponent(Entity entity);

	template<typename Component>
	static uint32_t GetComponentId();

private:
	static uint32_t entityCount;
	static std::shared_ptr<Entity[]> entities;

	static uint32_t componentCounter;
	static std::vector<ComponentPool> pools;

	static std::shared_ptr<EntityComponentMask[]> entityCompMasks;
};

template<typename Component>
Component* Registry::AddComponent(Entity entity) {
	unsigned int compId = GetComponentId<Component>();

	if (pools.size() == compId) {
		pools.push_back(ComponentPool(sizeof(Component)));
	}

	entityCompMasks[entity.id].set(compId);
	return pools[compId].InitComponent<Component>(entity);
}

template<typename Component>
Component* Registry::GetComponent(Entity entity) {
	unsigned int compId = GetComponentId<Component>();
	return pools[compId].GetComponent<Component>(entity);
}

template<typename Component>
uint32_t Registry::GetComponentId() {
	static uint32_t compId = componentCounter++;
	return compId;
}

