#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>

class Texture {
public:
    static std::shared_ptr<Texture> Load(const std::string& filePath);
    Texture() = delete;
    Texture(uint32_t width, uint32_t height, uint32_t id, const std::string& path);
    ~Texture();
    void Bind();
private:
	inline static std::unordered_map<std::string, std::weak_ptr<Texture>> m_ActiveTextures;

    uint32_t m_Width;
    uint32_t m_Height;
    uint32_t m_Id;
    std::string m_Path;
};
