#pragma once
#include <glm/glm.hpp>
#include "core/Base.h"

class DepthTexture {
public:
	DepthTexture() = default;
	DepthTexture(const u32 width, const u32 height);
	void Bind(const i32 textureUnit) const;
	void Resize(const glm::i32vec2 size) const;
	void AttachToActiveFrameBuffer() const;
	f32 ReadPixel(const glm::i32vec2& pixelCoord) const;
private:
	u32 m_Id;
};

