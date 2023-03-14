#include <cassert>
#include <stb/stb_image.h>
#include <glad/glad.h>
#include "CubeMap.h"
#include <iostream>

std::unordered_map<std::string, std::weak_ptr<CubeMap>> CubeMap::m_ActiveCubemaps;

std::shared_ptr<CubeMap> CubeMap::Load(const std::array<std::string, 6>& images) {
    std::string key = images[0];
    if (m_ActiveCubemaps.contains(key)) {
        return m_ActiveCubemaps[key].lock();
    }

    uint32_t id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    for (int i = 0; i < images.size(); i++) {
		int32_t width, height, numChannels;
		stbi_uc* imageData = stbi_load(images[i].c_str(), &width, &height, &numChannels, 0);

        assert(imageData);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

        stbi_image_free(imageData);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    auto cubemap = std::make_shared<CubeMap>(key, id);
    m_ActiveCubemaps[key] = cubemap;
    return cubemap;
}

CubeMap::CubeMap(const std::string& key, uint32_t id) 
    : m_Key(key), m_Id(id) { }

CubeMap::~CubeMap() {
    m_ActiveCubemaps.erase(m_Key);
    glDeleteTextures(1, &m_Id);
}

void CubeMap::Bind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id);
}
