#include "ComponentPool.h"
#include "EcsParams.h"

ComponentPool::ComponentPool(const u32 compSize) {
	m_CompSize = compSize;
	m_Buffer = MakeScope<u8[]>(compSize * MAX_ENTITIES);
}

void* ComponentPool::GetComponentAddress(const Entity entity) const {
	return (m_Buffer.get() + (m_CompSize * entity.id));
}
