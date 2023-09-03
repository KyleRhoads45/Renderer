#pragma once
#include "core/Base.h"

class Entity {
public:
	Entity() = default;
	Entity(const u32 id);
	u32 Id() const;
private:
	u32 m_Id;
};