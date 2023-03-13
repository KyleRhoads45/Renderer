﻿#include <cassert>
#include <stb/stb_image.h>
#include <glad/glad.h>
#include "Texture.h"

std::unordered_map<std::string, std::weak_ptr<Texture>> Texture::m_ActiveTextures;

std::shared_ptr<Texture> Texture::Load(const std::string& filePath) {
    if (m_ActiveTextures.contains(filePath)) {
        return m_ActiveTextures[filePath].lock();
    }

    stbi_set_flip_vertically_on_load(true);

    int32_t width;
    int32_t height;
    int32_t numChannels;
    stbi_uc* imageData = stbi_load(filePath.c_str(), &width, &height, &numChannels, 0);

    assert(imageData);

    uint32_t id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    if (numChannels == 1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, width, height, 0, GL_R16, GL_UNSIGNED_BYTE, imageData);
    }
    else if (numChannels == 2) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, imageData);
    }
    else if (numChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    }

    stbi_image_free(imageData);
    stbi_set_flip_vertically_on_load(false);

    auto texture = std::make_shared<Texture>(width, height, id, filePath);
    m_ActiveTextures[filePath] = texture;
    return texture;
}

Texture::Texture(uint32_t width, uint32_t height, uint32_t id, const std::string& path) 
    : m_Width(width), m_Height(height), m_Id(id), m_Path(path) { }

Texture::~Texture() {
    m_ActiveTextures.erase(m_Path);
    glDeleteTextures(1, &m_Id);
}

void Texture::Bind() {
	glBindTexture(GL_TEXTURE_2D, m_Id);
}
