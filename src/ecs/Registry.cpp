#include "Registry.h"

uint32_t Registry::entityCount = 0;
uint32_t Registry::componentCounter = 0;
std::vector<ComponentPool> Registry::pools;
std::shared_ptr<Entity[]> Registry::entities = std::make_shared<Entity[]>(MAX_ENTITIES);
std::shared_ptr<EntityComponentMask[]> Registry::entityCompMasks = std::make_unique<EntityComponentMask[]>(MAX_ENTITIES);

Entity Registry::Create() {
    return entities[entityCount++];
}
