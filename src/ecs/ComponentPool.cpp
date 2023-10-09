#include "ComponentPool.h"

ComponentPool::ComponentPool(const u32 compSize) {
	m_NextCompIndex = 0;
	m_ComponentOffsetsSize = 0;
	m_CurBufferSize = 1;
	m_CompSize = compSize;
	
	m_Buffer = MakeRef<u8[]>(compSize * m_CurBufferSize);
	m_ComponentOffsets = MakeRef<u32[]>(m_CurBufferSize);
}

void* ComponentPool::GetComponentAddress(const u32 index) const {
	return (m_Buffer.get() + (m_CompSize * m_ComponentOffsets[index]));
}