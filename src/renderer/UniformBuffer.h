#pragma once
#include <array>
#include <string>
#include <unordered_map>

static struct VariableData {
	uint32_t m_StartOffset;
	uint32_t m_DataSize;

	VariableData() = default;
	VariableData(uint32_t offset, uint32_t dataSize) {
		m_StartOffset = offset;
		m_DataSize = dataSize;
	}
};

class UniformBuffer {
public:
	UniformBuffer() = default;
	UniformBuffer(uint32_t bindingPoint);
	void Register(const std::string& variable, size_t sizeInBytes);
	void FinishedRegistering();
	void SubBufferData(const std::string& variable, void* data);
private:
	void MoveSizeToNextOpenChunk();

	std::unordered_map<std::string, VariableData> m_StartOffsetLookUp;

	size_t m_BufferSizeInBytes;
	uint32_t m_Id;
};
