#include "View.h"

ViewIterator::ViewIterator(const u32 startIndex, const EntityCompMask mask) {
	m_Index = startIndex;
	m_Mask = mask;
}

Entity ViewIterator::operator*() const {
	return Registry::m_Entities[m_Index];
}

ViewIterator& ViewIterator::operator++() {
	m_Index++;
	while (m_Index < Registry::GetEntityCount() && !MaskIsSubset(m_Mask, Registry::m_EntityCompMasks[m_Index])) {
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

bool ViewIterator::MaskIsSubset(const EntityCompMask& subSet, const EntityCompMask& superSet) {
	return ((subSet & superSet) == subSet);
}

