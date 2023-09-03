#pragma once
#include "Registry.h"

class ViewIterator {
public:
	ViewIterator(u32 startIndex, EntityCompMask mask);
	Entity operator*() const;
	ViewIterator& operator++();
	bool operator==(const ViewIterator& other) const;
	bool operator!=(const ViewIterator& other) const;
private:
	bool MaskIsSubset(const EntityCompMask& subSet, const EntityCompMask& superSet);
private:
	u32 m_Index;
	EntityCompMask m_Mask;
};

template<typename... Components>
class View {
public:
	View() {
		u32 componentIds[] = {Registry::GetComponentId<Components>()...};
		for (int i = 0; i < sizeof...(Components); i++) {
			mask.set(componentIds[i]);
		}
	}

	const ViewIterator begin() const {
		u32 startIndex = 0;
		while (startIndex < Registry::GetEntityCount() && Registry::m_EntityCompMasks[startIndex] != mask) {
			startIndex++;
		}
		return ViewIterator(startIndex, mask);
	}

	const ViewIterator end() const {
		return ViewIterator(Registry::GetEntityCount(), mask);
	}
private:
	EntityCompMask mask;
};
