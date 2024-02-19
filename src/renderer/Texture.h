#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "core/Base.h"

class Texture {
public:
    static Ref<Texture> Load(const std::string& filePath);
    Texture() = delete;
    Texture(u32 width, u32 height, u32 id, const std::string& path);
    ~Texture();
    void Bind();
private:
	inline static std::unordered_map<std::string, std::weak_ptr<Texture>> m_ActiveTextures;

    u32 m_Width;
    u32 m_Height;
    u32 m_Id;
    std::string m_Path;
};
