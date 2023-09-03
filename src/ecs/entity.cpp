#include "Entity.h"

Entity::Entity(const u32 id) {
    m_Id = id;    
}

u32 Entity::Id() const {
    return m_Id;
}
