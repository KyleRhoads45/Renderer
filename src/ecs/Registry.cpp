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
