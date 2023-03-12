#pragma once
#include <cstdint>

class DepthTexture {
public:
	DepthTexture() = default;
	DepthTexture(const uint32_t width, const uint32_t height);
	void Bind();
	void AttachToActiveFrameBuffer();
private:
	uint32_t m_Id;
};

