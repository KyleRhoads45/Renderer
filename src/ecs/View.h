#pragma once
#include "Registry.h"

class ViewIterator {
public:
	ViewIterator(uint32_t startIndex, EntityComponentMask mask);
	Entity operator*() const;
	ViewIterator& operator++();
	bool operator==(const ViewIterator& other) const;
	bool operator!=(const ViewIterator& other) const;
private:
	uint32_t index;
	EntityComponentMask mask;
};

template<typename... Components>
class View {
public:
	View() {
		uint32_t componentIds[] = {Registry::GetComponentId<Components>()...};
		for (int i = 0; i < sizeof...(Components); i++) {
			mask.set(componentIds[i]);
		}
	};

	const ViewIterator begin() const {
		uint32_t startIndex = 0;
		while (startIndex < Registry::entityCount && Registry::entityCompMasks[startIndex] != mask) {
			startIndex++;
		}
		return ViewIterator(startIndex, mask);
	}

	const ViewIterator end() const {
		return ViewIterator(Registry::entityCount, mask);
	}

private:
	EntityComponentMask mask;
};
