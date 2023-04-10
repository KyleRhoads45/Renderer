#pragma once
#include <array>
#include <string>
#include <unordered_map>
#include "core/Base.h"

struct VariableData {
	u32 m_StartOffset;
	u32 m_DataSize;

	VariableData() = default;
	VariableData(u32 offset, u32 dataSize) {
		m_StartOffset = offset;
		m_DataSize = dataSize;
	}
};

class UniformBuffer {
public:
	UniformBuffer(u32 bindingPoint);
	~UniformBuffer();
	void Register(const std::string& variable, u32 sizeInBytes);
	void FinishedRegistering();
	void SubBufferData(const std::string& variable, void* data);
private:
	void MoveSizeToNextOpenChunk();

	std::unordered_map<std::string, VariableData> m_StartOffsetLookUp;

	u32 m_BufferSizeInBytes;
	u32 m_Id;
};
