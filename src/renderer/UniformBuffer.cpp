#include <cassert>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(uint32_t bindingPoint) : m_BufferSizeInBytes(0) {
	glGenBuffers(1, &m_Id);
	glBindBuffer(GL_UNIFORM_BUFFER, m_Id);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_Id);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::Register(const std::string& variable, size_t sizeInBytes) {
	constexpr size_t floatSize = sizeof(float);
	if (sizeInBytes <= floatSize) {
		m_StartOffsetLookUp[variable] = VariableData(m_BufferSizeInBytes, sizeInBytes);
		m_BufferSizeInBytes += sizeInBytes;
		return;
	}

	MoveSizeToNextOpenChunk();
	m_StartOffsetLookUp[variable] = VariableData(m_BufferSizeInBytes, sizeInBytes);
	m_BufferSizeInBytes += sizeInBytes;
}

void UniformBuffer::FinishedRegistering() {
	MoveSizeToNextOpenChunk();
	glBindBuffer(GL_UNIFORM_BUFFER, m_Id);
	glBufferData(GL_UNIFORM_BUFFER, m_BufferSizeInBytes, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::SubBufferData(const std::string& variable, void* data) {
	VariableData varData = m_StartOffsetLookUp[variable];

	glBindBuffer(GL_UNIFORM_BUFFER, m_Id);
	glBufferSubData(GL_UNIFORM_BUFFER, varData.m_StartOffset, varData.m_DataSize, data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::MoveSizeToNextOpenChunk() {
	const uint32_t ChunkSize = 16;
	const uint32_t ByteSize = 4;

	uint32_t bytesIntoChunk = m_BufferSizeInBytes % ChunkSize;
	if (bytesIntoChunk == 0) return;

	m_BufferSizeInBytes += ChunkSize - bytesIntoChunk;
}
