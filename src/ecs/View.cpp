#include "View.h"

ViewIterator::ViewIterator(u32 startIndex, EntityCompMask mask) {
	m_Index = startIndex;
	m_Mask = mask;
}

Entity ViewIterator::operator*() const {
	return Registry::entities[m_Index];
}

ViewIterator& ViewIterator::operator++() {
	m_Index++;
	while (m_Index < Registry::entityCount && Registry::entityCompMasks[m_Index] != m_Mask) {
		m_Index++;
	}
	return *this;
}

bool ViewIterator::operator==(const ViewIterator& other) const {
	return (m_Index == other.m_Index && m_Mask == other.m_Mask);
}

bool ViewIterator::operator!=(const ViewIterator& other) const {
	return (m_Index != other.m_Index || m_Mask != other.m_Mask);
}

