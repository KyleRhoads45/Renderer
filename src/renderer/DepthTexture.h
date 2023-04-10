#pragma once
#include "core/Base.h"

class DepthTexture {
public:
	DepthTexture() = default;
	DepthTexture(const u32 width, const u32 height);
	void Bind();
	void AttachToActiveFrameBuffer();
private:
	u32 m_Id;
};

