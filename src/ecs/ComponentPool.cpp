#include "ComponentPool.h"
#include "EcsParams.h"

ComponentPool::ComponentPool(const size_t componentSize)
	: componentSize(componentSize)
{
	buffer = std::make_unique<std::byte[]>(componentSize * MAX_ENTITIES);
}

void* ComponentPool::GetComponentAddress(const Entity entity) const {
	return (buffer.get() + (componentSize * entity.id));
}
