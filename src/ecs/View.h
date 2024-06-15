#pragma once
#include "Registry.h"

enum class ViewFilter { With, Only, WithWithout};

class ViewIterator {
public:
	ViewIterator(const EntityCompMask mask, const EntityCompMask excludeMask, ViewFilter filter);
	void SeekBegin();
	void SeekEnd();
	Entity& operator*() const;
	ViewIterator& operator++();
	bool operator==(const ViewIterator& other) const;
	bool operator!=(const ViewIterator& other) const;
private:
	void FindNextIndex(const size_t startIndex);
private:
	size_t m_Index;
	EntityCompMask m_Mask;
	EntityCompMask m_ExcludeMask;
	ViewFilter m_Filter;
};

template<typename... Components>
class View {
public:

	View() {
		mask = EntityCompMask::From<Components...>();
		filter = ViewFilter::With;
	};

	template<typename... Excluded>
	View Exclude() {
		excludeMask = EntityCompMask::From<Excluded...>();
		filter = ViewFilter::WithWithout;
		return *this;
	}

	template<typename... OnlyComps>
	View Only() {
		mask = EntityCompMask::From<OnlyComps...>();
		filter = ViewFilter::Only;
		return *this;
	}

	const ViewIterator begin() const {
		ViewIterator iterator(mask, excludeMask, filter);
		iterator.SeekBegin();
		return iterator;
	}

	const ViewIterator end() const {
		ViewIterator iterator(mask, excludeMask, filter);
		iterator.SeekEnd();
		return iterator;
	}

private:
	EntityCompMask mask;
	EntityCompMask excludeMask;
	ViewFilter filter;
};
