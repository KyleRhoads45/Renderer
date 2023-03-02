#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Texture.h"

class Shader {
public:
    Shader();
    Shader(const std::string& vertFile, const std::string& fragFile);
    void Bind() const;
    void SetMat4(const std::string& name, const glm::mat4& mat4) const;

    unsigned int shaderId; 
private:
    void CreateShader(const std::string& vertFile, const std::string& fragFile);
    std::string LoadShaderFile(const std::string& filePath) const;
    void CheckCompileErrors(unsigned int shader, const std::string& type) const;
};