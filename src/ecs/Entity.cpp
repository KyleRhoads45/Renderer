#include "Entity.h"

static unsigned int entityId = 0;

Entity::Entity() 
	: id(entityId++) {}
