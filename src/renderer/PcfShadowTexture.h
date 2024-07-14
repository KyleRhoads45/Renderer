#pragma once
#include "core/Base.h"

class PcfShadowTexture {
public:
    PcfShadowTexture() = default;
    PcfShadowTexture(i32 windowSize, i32 filterSize);
    ~PcfShadowTexture();

    void Bind(const i32 textureUnit) const;

    i32 WindowSize() const { return m_WindowSize; }
    i32 FilterSize() const { return m_FilterSize; }
private:
    std::vector<glm::vec2> CreateTexels() const;
    glm::vec2 CalculateSamplingPosition(i32 x, i32 y) const;
private:
    i32 m_WindowSize;
    i32 m_FilterSize;
    u32 m_Id;
};
