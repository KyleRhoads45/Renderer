#pragma once
#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include "core/Base.h"

class Texture {
public:
    static Ref<Texture> Load(const std::string& filePath);
public:
    Texture() = delete;
    Texture(const u32 width, const u32 height, const u32 id, const std::string& path);
    ~Texture();

    void Bind() const { glBindTexture(GL_TEXTURE_2D, m_Id); }

    std::string Path() const { return m_Path; }
    bool HasTransparency() const { return m_HasTransparency; }
private:
	inline static std::unordered_map<std::string, std::weak_ptr<Texture>> m_ActiveTextures;
private:
    u32 m_Width;
    u32 m_Height;
    u32 m_Id;
    std::string m_Path;
    bool m_HasTransparency;
};
