#include <cassert>
#include <stb/stb_image.h>
#include "Texture.h"
#include <iostream>

Ref<Texture> Texture::Load(const std::string& filePath, const Type texType) {
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

    i32 format = GetFormat(numChannels);
    i32 internalFormat = GetInternalFormat(texType, numChannels);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
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

i32 Texture::GetFormat(const i32 numChannels) {
    switch (numChannels) {
        case 1:
            return GL_R16;
        case 2:
            return GL_RG;
        case 3:
            return GL_RGB;
        default:
            return GL_RGBA;
    }
}

i32 Texture::GetInternalFormat(const Type format, const i32 numChannels) {
    switch (numChannels) {
        case 1:
            return GL_R16;
        case 2:
            return GL_RG;
        case 3:
            switch (format) {
                case Type::Default:
                     return GL_SRGB;
                case Type::NormalMap:
                    return GL_RGB;
            }
        default:
            switch (format) {
                case Type::Default:
                     return GL_SRGB_ALPHA;
                case Type::NormalMap:
                    return GL_RGBA;
            }
    }
    return GL_SRGB;
}
