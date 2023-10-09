#include <limits>
#include "Registry.h"

Entity Registry::Create() {
    const Entity newEntity(m_Entities.size());
    m_Entities.push_back(newEntity);
    m_EntityCompMasks.emplace_back();
    return newEntity;
}

size_t Registry::GetEntityCount() {
    return m_Entities.size();
}

Entity::Entity(const u32 id) 
    : m_Id(id) { }

u32 Entity::Id() const {
    return m_Id;
}

bool Entity::operator==(const Entity& other) const {
    return m_Id == other.Id();
}

Entity Entity::Null() {
    return Entity(UINT32_MAX);
}

bool EntityCompMask::operator==(const EntityCompMask& otherMask) const {
	return m_Mask == otherMask.m_Mask;
}

bool EntityCompMask::Empty() {
	return m_Mask.none();
}

bool EntityCompMask::IsSubsetOf(const EntityCompMask& superSet) const {
	return (m_Mask & superSet.m_Mask) == m_Mask;
}

bool EntityCompMask::SharesAnyWith(const EntityCompMask& otherMask) const {
	return (m_Mask & otherMask.m_Mask).any();
}

void EntityCompMask::Set(u32 index) {
	m_Mask.set(index);
}

bool EntityCompMask::Test(u32 index) {
	return m_Mask.test(index);
}
