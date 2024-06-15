#pragma once
#include "Bounds.h"
#include "Bounds.h"
#include "Bounds.h"
#include "Bounds.h"
#include "core/Base.h"
#include "DepthTexture.h"

class FrameBuffer {
public:
	enum Format { RGB, RED_INTEGER };
	FrameBuffer() = default;
	FrameBuffer(glm::i32vec2 size, Format format);
	void Bind();
	void UnBind() const;
	void RedIntegerFill(i32 fillValue) const;
	void Resize(glm::i32vec2 size);
	i32 ReadPixel(const glm::i32vec2& pixelCoord) const;
	f32 ReadDepth(const glm::i32vec2& pixelCoord) const;
	glm::i32vec2 Size() const;
	u32 Texture() const;
private:
	u32 m_Fbo;
	u32 m_Texture;
	Format m_Format;
	glm::i32vec2 m_Size;
	glm::i32vec2 m_PrevViewportSize;
	DepthTexture m_DepthTexture;
};

