#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/ext/scalar_constants.hpp>
#include "PcfShadowTexture.h"

PcfShadowTexture::PcfShadowTexture(const i32 windowSize, const i32 filterSize)
    : m_WindowSize(windowSize), m_FilterSize(filterSize)
{
    std::vector<glm::vec2> texels = CreateTexels();

    glGenTextures(1, &m_Id);
    glBindTexture(GL_TEXTURE_3D, m_Id);
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    i32 filterSqr = m_FilterSize * m_FilterSize;
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RG32F, filterSqr, m_WindowSize, m_WindowSize, 0, GL_RG, GL_FLOAT, texels.data());
    glBindTexture(GL_TEXTURE_3D, 0);
}

PcfShadowTexture::~PcfShadowTexture() {
   glDeleteTextures(1, &m_Id); 
}

void PcfShadowTexture::Bind(const i32 textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_3D, m_Id);
}

std::vector<glm::vec2> PcfShadowTexture::CreateTexels() const {
    i32 windowSqr = m_WindowSize * m_WindowSize;
    i32 filterSqr = m_FilterSize * m_FilterSize;
    
    std::vector<glm::vec2> offsets;
    offsets.resize(windowSqr * filterSqr);

    i32 index = 0;

    for (i32 i = 0; i < windowSqr; i++) {
        // Reverse Y so outermost samples of disc are first in the filter
        for (i32 y = m_FilterSize - 1; y >= 0; y--) {
            for (i32 x = 0; x < m_FilterSize; x++) {
                offsets[index] = CalculateSamplingPosition(x, y);
                index++;
            }
        }
    }

    return offsets;
}

glm::vec2 PcfShadowTexture::CalculateSamplingPosition(i32 x, i32 y) const {
    f32 u = static_cast<f32>(x);
    f32 v = static_cast<f32>(y);

    f32 uJitter = glm::linearRand(-0.5f, 0.5f);
    f32 vJitter = glm::linearRand(-0.5f, 0.5f);
    
    u = (u + 0.5f + uJitter) / m_FilterSize;
    v = (v + 0.5f + vJitter) / m_FilterSize;

    // Warp point to disk pattern
    float xOffset = glm::sqrt(v) * glm::cos(2.0f * glm::pi<f32>() * u);
    float yOffset = glm::sqrt(v) * glm::sin(2.0f * glm::pi<f32>() * u);
    
    return { xOffset, yOffset };
}
