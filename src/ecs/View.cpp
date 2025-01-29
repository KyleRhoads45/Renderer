#include "View.h"

ViewIterator::ViewIterator(Registry& registry, const EntityCompMask mask, const EntityCompMask excludeMask, ViewFilter filter) 
	: m_Registry(registry), m_Index(0), m_Mask(mask),
      m_ExcludeMask(excludeMask), m_Filter(filter) { }

void ViewIterator::SeekBegin() {
	FindNextIndex(0);
}

void ViewIterator::SeekEnd() {
	m_Index = m_Registry.GetEntityCount();
}

Entity& ViewIterator::operator*() const {
	return m_Registry.m_Entities[m_Index];
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

void ViewIterator::FindNextIndex(const size_t startIndex) {
	for (size_t i = startIndex; i < m_Registry.GetEntityCount(); i++) {
		const EntityCompMask& entityMask = m_Registry.m_EntityCompMasks[i];

		switch (m_Filter) 
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

	m_Index = m_Registry.GetEntityCount();
}
