#include "View.h"

ViewIterator::ViewIterator(uint32_t startIndex, EntityComponentMask mask)
	: index(startIndex), mask(mask) { }

Entity ViewIterator::operator*() const {
	return Registry::entities[index];
}

ViewIterator& ViewIterator::operator++() {
	index++;
	while (index < Registry::entityCount && Registry::entityCompMasks[index] != mask) {
		index++;
	}
	return *this;
}

bool ViewIterator::operator==(const ViewIterator& other) const {
	return (index == other.index && mask == other.mask);
}

bool ViewIterator::operator!=(const ViewIterator& other) const {
	return (index != other.index || mask != other.mask);
}

