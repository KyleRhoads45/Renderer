#pragma once
#include <cmath>
#include "core/Base.h"

class ComponentPool {
public:
	ComponentPool(const u32 compSize);

	template<typename Component>
	Component& Get(const u32 index);

	template<typename Component>
	Component& Add(const u32 index);
private:
	void* GetComponentAddress(const u32 index) const;
private:
	u32 m_CompSize;
	u32 m_NextCompIndex;
	
	u32 m_CurBufferSize;
	Ref<u8[]> m_Buffer;

	u32 m_ComponentOffsetsSize;
	Ref<u32[]> m_ComponentOffsets;
};

template <typename Component>
Component& ComponentPool::Get(const u32 index) {
	return *static_cast<Component*>(GetComponentAddress(index));
}

template <typename Component>
Component& ComponentPool::Add(const u32 index) {
	if (index >= m_ComponentOffsetsSize) {
		const u32 oldSize = m_ComponentOffsetsSize;
		m_ComponentOffsetsSize = index + 1;
		
		const Ref<u32[]> tempComponentOffsets = m_ComponentOffsets;
		m_ComponentOffsets = MakeRef<u32[]>(m_ComponentOffsetsSize);
		memcpy(m_ComponentOffsets.get(), tempComponentOffsets.get(), sizeof(u32) * oldSize);
	}

	if (m_NextCompIndex >= m_CurBufferSize) {
		const u32 oldSize = m_CurBufferSize;
		
		constexpr f32 resizeScaler = 1.5;
		m_CurBufferSize = std::ceil(m_CurBufferSize * resizeScaler);

		const Ref<u8[]> tempBuffer = m_Buffer;
		m_Buffer = MakeRef<u8[]>(m_CompSize * m_CurBufferSize);
		memcpy(m_Buffer.get(), tempBuffer.get(), m_CompSize * oldSize);
	}
	
	m_ComponentOffsets[index] = m_NextCompIndex;
	m_NextCompIndex++;
	
	Component* component = new (GetComponentAddress(index))Component;
	return *component;
}
