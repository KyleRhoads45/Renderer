#include "View.h"

ViewIterator::ViewIterator(const EntityCompMask mask, const EntityCompMask excludeMask, ViewFilter filter) 
	: m_Index(0), m_Mask(mask), m_ExcludeMask(excludeMask), m_filter(filter) { }

void ViewIterator::SeekBegin() {
	FindNextIndex(0);
}

void ViewIterator::SeekEnd() {
	m_Index = Registry::GetEntityCount();
}

Entity ViewIterator::operator*() const {
	return Registry::m_Entities[m_Index];
}

ViewIterator& ViewIterator::operator++() {
	m_Index++;
	FindNextIndex(m_Index);
	return *this;
}

bool ViewIterator::operator==(const ViewIterator& other) const {
	return (m_Index == other.m_Index && m_Mask.m_Mask == other.m_Mask.m_Mask);
}

bool ViewIterator::operator!=(const ViewIterator& other) const {
	return (m_Index != other.m_Index || m_Mask.m_Mask != other.m_Mask.m_Mask);
}

void ViewIterator::FindNextIndex(int startIndex) {
	for (u32 i = startIndex; i < Registry::GetEntityCount(); i++) {
		const EntityCompMask& entityMask = Registry::m_EntityCompMasks[i];

		switch (m_filter) 
		{
		case ViewFilter::With:
			if (m_Mask.IsSubsetOf(entityMask)) {
				m_Index = i;
				return;
			}
			break;

		case ViewFilter::Only:
			if (m_Mask == entityMask) {
				m_Index = i;
				return;
			}
			break;

		case ViewFilter::WithWithout:
			if (m_Mask.IsSubsetOf(entityMask) && !m_ExcludeMask.SharesAnyWith(entityMask)) {
				m_Index = i;
				return;
			}
			break;
		}

	}

	m_Index = Registry::GetEntityCount();
}
