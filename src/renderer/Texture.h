#pragma once
#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include "core/Base.h"

class Texture {
public:
    enum class Type { Default, NormalMap };
    static Ref<Texture> Load(const std::string& filePath, const Type texType);
public:
    Texture() = delete;
    Texture(const u32 width, const u32 height, const u32 id, const std::string& path);
    ~Texture();

    void Bind() const { glBindTexture(GL_TEXTURE_2D, m_Id); }

    std::string Path() const { return m_Path; }
    bool HasTransparency() const { return m_HasTransparency; }
private:
    static i32 GetFormat(const i32 numChannels);
    static i32 GetInternalFormat(const Type format, const i32 numChannels);
private:
	inline static std::unordered_map<std::string, std::weak_ptr<Texture>> m_ActiveTextures;
private:
    u32 m_Width;
    u32 m_Height;
    u32 m_Id;
    std::string m_Path;
    bool m_HasTransparency;
};
