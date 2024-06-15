#pragma once
#include <cmath>
#include "core/Base.h"

class ComponentPool {
public:
	ComponentPool(const u32 compSize);

	template<typename Component>
	Component& Get(const size_t index);

	template<typename Component>
	Component& Add(const size_t index);
private:
	void* GetComponentAddress(const size_t index) const;
private:
	u32 m_CompSize;
	u32 m_NextCompIndex;
	
	size_t m_CurBufferSize;
	Ref<u8[]> m_Buffer;

	size_t m_ComponentOffsetsSize;
	Ref<u32[]> m_ComponentOffsets;
};

template <typename Component>
Component& ComponentPool::Get(const size_t index) {
	return *static_cast<Component*>(GetComponentAddress(index));
}

template <typename Component>
Component& ComponentPool::Add(const size_t index) {
	if (index >= m_ComponentOffsetsSize) {
		const size_t oldSize = m_ComponentOffsetsSize;
		m_ComponentOffsetsSize = index + 1;
		
		const Ref<u32[]> tempComponentOffsets = m_ComponentOffsets;
		m_ComponentOffsets = MakeRef<u32[]>(m_ComponentOffsetsSize);
		memcpy(m_ComponentOffsets.get(), tempComponentOffsets.get(), sizeof(u32) * oldSize);
	}

	if (m_NextCompIndex >= m_CurBufferSize) {
		const size_t oldSize = m_CurBufferSize;
		
		constexpr f32 resizeScaler = 1.5;
		m_CurBufferSize = static_cast<size_t>(std::ceil(m_CurBufferSize * resizeScaler));

		const Ref<u8[]> tempBuffer = m_Buffer;
		m_Buffer = MakeRef<u8[]>(m_CompSize * m_CurBufferSize);
		memcpy(m_Buffer.get(), tempBuffer.get(), m_CompSize * oldSize);
	}
	
	m_ComponentOffsets[index] = m_NextCompIndex;
	m_NextCompIndex++;
	
	Component* component = new (GetComponentAddress(index))Component;
	return *component;
}
