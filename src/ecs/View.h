#pragma once
#include "Registry.h"

enum class ViewFilter { With, Only, WithWithout};

class ViewIterator {
public:
	ViewIterator(Registry& registry, const EntityCompMask mask, const EntityCompMask excludeMask, ViewFilter filter);
	void SeekBegin();
	void SeekEnd();
	Entity& operator*() const;
	ViewIterator& operator++();
	bool operator==(const ViewIterator& other) const;
	bool operator!=(const ViewIterator& other) const;
private:
	void FindNextIndex(const size_t startIndex);
private:
	Registry& m_Registry;
	size_t m_Index;
	EntityCompMask m_Mask;
	EntityCompMask m_ExcludeMask;
	ViewFilter m_Filter;
};

template<typename... Components>
class View {
public:

	View(Registry& registry) : m_Registry(registry), m_Filter(ViewFilter::With)
	{
		m_Mask = EntityCompMask::From<Components...>(m_Registry);
	}

	View Only() {
		m_Mask = EntityCompMask::From<Components...>(m_Registry);
		m_Filter = ViewFilter::Only;
		return *this;
	}

	template<typename... Excluded>
	View Exclude() {
		m_ExcludeMask = EntityCompMask::From<Excluded...>(m_Registry);
		m_Filter = ViewFilter::WithWithout;
		return *this;
	}

	const ViewIterator begin() const {
		ViewIterator iterator(m_Registry, m_Mask, m_ExcludeMask, m_Filter);
		iterator.SeekBegin();
		return iterator;
	}

	const ViewIterator end() const {
		ViewIterator iterator(m_Registry, m_Mask, m_ExcludeMask, m_Filter);
		iterator.SeekEnd();
		return iterator;
	}

private:
	Registry& m_Registry;
	EntityCompMask m_Mask;
	EntityCompMask m_ExcludeMask;
	ViewFilter m_Filter;
};
