#pragma once
#include <glm/glm.hpp>
#include "core/Base.h"

class DepthTexture {
public:
	DepthTexture() = default;
	DepthTexture(const u32 width, const u32 height);
	void Bind();
	void Resize(const glm::i32vec2 size);
	void AttachToActiveFrameBuffer();
private:
	u32 m_Id;
};

