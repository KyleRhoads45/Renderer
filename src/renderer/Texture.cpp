#include <cassert>
#include <stb/stb_image.h>
#include "Texture.h"
#include <iostream>

Ref<Texture> Texture::Load(const std::string& filePath) {
    if (m_ActiveTextures.contains(filePath)) {
        return m_ActiveTextures[filePath].lock();
    }

    stbi_set_flip_vertically_on_load(true);

    i32 width;
    i32 height;
    i32 numChannels;
    stbi_uc* imageData = stbi_load(filePath.c_str(), &width, &height, &numChannels, 0);

    assert(imageData);

    u32 id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(imageData);
    stbi_set_flip_vertically_on_load(false);

    auto texture = MakeRef<Texture>(width, height, id, filePath);
    m_ActiveTextures[filePath] = texture;
    texture->m_HasTransparency = numChannels == 4;
    return texture;
}

Texture::Texture(const u32 width, const u32 height, const u32 id, const std::string& path)
    : m_Width(width), m_Height(height), m_Id(id), m_Path(path), m_HasTransparency(false) { }

Texture::~Texture() {
    m_ActiveTextures.erase(m_Path);
    glDeleteTextures(1, &m_Id);
}