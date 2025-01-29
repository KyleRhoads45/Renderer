#include "Registry.h"

Entity Registry::Create() {
    const Entity newEntity(m_Entities.size());
    m_Entities.push_back(newEntity);
    m_EntityCompMasks.emplace_back();
    return newEntity;
}

void Registry::FreeAll() {
	m_Entities.clear();
	m_Pools.clear();
	m_EntityCompMasks.clear();
}

size_t Registry::GetEntityCount() {
    return m_Entities.size();
}

Entity::Entity(const size_t id) : m_Id(id) { }

size_t Entity::Id() const {
    return m_Id;
}

bool Entity::operator==(const Entity& other) const {
    return m_Id == other.Id();
}

Entity Entity::Null() {
    return static_cast<size_t>(-1);
}

bool EntityCompMask::operator==(const EntityCompMask& otherMask) const {
	return m_Mask == otherMask.m_Mask;
}

bool EntityCompMask::Empty() const {
	return m_Mask.none();
}

bool EntityCompMask::IsSubsetOf(const EntityCompMask& superSet) const {
	return (m_Mask & superSet.m_Mask) == m_Mask;
}

bool EntityCompMask::SharesAnyWith(const EntityCompMask& otherMask) const {
	return (m_Mask & otherMask.m_Mask).any();
}

void EntityCompMask::Set(const u32 index) {
	m_Mask.set(index);
}

bool EntityCompMask::Test(const u32 index) const {
	return m_Mask.test(index);
}