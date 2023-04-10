#include "Registry.h"

Entity Registry::Create() {
    int count = entityCount++;
    entities[count].id = count;
    return entities[count];
}
