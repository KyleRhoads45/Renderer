#pragma once
#include <cstdint>

struct Texture {
    Texture() = delete;
    Texture(const char* filePath);

    int width, height;
    uint32_t id;
};
