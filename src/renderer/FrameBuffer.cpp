#include <glad/glad.h>
#include "editor/Editor.h"
#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(glm::i32vec2 size, Format format) 
	: m_Size(size), m_Format(format)
{
	glGenFramebuffers(1, &m_Fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);

	switch (m_Format) {
		case RGB:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, m_Size.x, m_Size.y, 0, GL_RGB, GL_UNSIGNED_INT, NULL);
			break;
		case RED_INTEGER:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, m_Size.x, m_Size.y, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
			break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);

	m_DepthTexture = DepthTexture(m_Size.x, m_Size.y);
	m_DepthTexture.AttachToActiveFrameBuffer();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Bind() {
	i32 viewportDimensions[4];
	glGetIntegerv(GL_VIEWPORT, viewportDimensions);
	m_PrevViewportSize.x = viewportDimensions[2];
	m_PrevViewportSize.y = viewportDimensions[3];

	glViewport(0, 0, m_Size.x, m_Size.y);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::UnBind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_PrevViewportSize.x, m_PrevViewportSize.y);
}

void FrameBuffer::RedIntegerFill(const i32 fillValue) const {
	glClearTexImage(m_Texture, 0, GL_RED_INTEGER, GL_INT, &fillValue);
}

void FrameBuffer::Resize(const glm::i32vec2 size) {
	m_Size = size;
	m_DepthTexture.Resize(size);

	glBindTexture(GL_TEXTURE_2D, m_Texture);
	switch (m_Format) {
		case RGB:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Size.x, m_Size.y, 0, GL_RGB, GL_UNSIGNED_INT, nullptr);
			break;
		case RED_INTEGER:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, m_Size.x, m_Size.y, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
			break;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

i32 FrameBuffer::ReadPixel(const glm::i32vec2& pixelCoord) const {
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	i32 pixel;
	glReadPixels(pixelCoord.x, pixelCoord.y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);
	return pixel;
}

f32 FrameBuffer::ReadDepth(const glm::i32vec2& pixelCoord) const {
	return m_DepthTexture.ReadPixel(pixelCoord);
}

glm::i32vec2 FrameBuffer::Size() const {
	return m_Size;
}

u32 FrameBuffer::Texture() const {
	return m_Texture;
}
