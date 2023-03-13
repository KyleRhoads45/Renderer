#pragma once
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include "Texture.h"

class Shader {
public:
    Shader() = default;
    Shader(const std::string& vertFile, const std::string& fragFile);
    void Bind() const;
    void SetInt(const std::string& name, int num);
    void SetFloat(const std::string& name, float num);
    void SetMat4(const std::string& name, const glm::mat4& mat4);
private:
    inline int GetUniformLocation(const std::string& name);
    void CreateShader(const std::string& vertFile, const std::string& fragFile);
    std::string LoadShaderFile(const std::string& filePath) const;
    void CheckCompileErrors(uint32_t shader, const std::string& type) const;

    uint32_t shaderId; 
    std::unordered_map<std::string, int> m_UniformLocations;
};