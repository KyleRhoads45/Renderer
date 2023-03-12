#pragma once
#include <cstdint>

struct Texture {
    Texture() = default;
    Texture(const char* filePath);
    void Bind();

    int width, height;
    uint32_t id;
};
