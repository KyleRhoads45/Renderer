#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "Shader.h"

Shader::Shader(const std::string& vertFile, const std::string& fragFile) {
    CreateShader(vertFile, fragFile);
}

void Shader::Bind() const {
    glUseProgram(shaderId);
}

void Shader::SetInt(const std::string& name, int num) {
    glUniform1i(GetUniformLocation(name), num);
}

void Shader::SetFloat(const std::string& name, float num) {
    glUniform1f(GetUniformLocation(name), num);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat4) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat4));
}

inline int Shader::GetUniformLocation(const std::string& name) {
    if (!m_UniformLocations.contains(name)) {
        int location = glGetUniformLocation(shaderId, name.c_str());
        m_UniformLocations[name] = location;
        return location;
    }

    return m_UniformLocations[name];
}

void Shader::CreateShader(const std::string& vertFile, const std::string& fragFile) {
    const std::string vertCodeString = LoadShaderFile(vertFile);
    const std::string fragCodeString = LoadShaderFile(fragFile);

    const char* vertCode = vertCodeString.c_str();
    const char* fragCode = fragCodeString.c_str();

    // Vertex shader
    const unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertCode, NULL);
    glCompileShader(vertex);

    #if _DEBUG
    CheckCompileErrors(vertex, "Vertex");
    #endif

    // Fragment Shader
    const unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragCode, NULL);
    glCompileShader(fragment);

    #if _DEBUG
    CheckCompileErrors(fragment, "Fragment");
    #endif

    // Shader Program
    shaderId = glCreateProgram();
    glAttachShader(shaderId, vertex);
    glAttachShader(shaderId, fragment);
    glLinkProgram(shaderId);

    #if _DEBUG
    CheckCompileErrors(shaderId, "Shader Linking");
    #endif

    // Delete the shaders as they're linked into our program and are no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    Bind();
}

// Loads a shader file into a string
std::string Shader::LoadShaderFile(const std::string& filePath) const {
    std::fstream file(filePath);
    std::string codeString;

    if (file.fail()) {
        std::cout << "Failed to open " << filePath << std::endl;
        return codeString;
    }

    std::stringstream stream;
    stream << file.rdbuf();
    codeString = stream.str();

    file.close();
    return codeString;
}

// Checks for compilation errors in the shaders
void Shader::CheckCompileErrors(uint32_t shader, const std::string& type) const {
    int success;
    char infoLog[1024];

    if (type == "Shader Linking") {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);

        if (success) return;

        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "Error within " << type << std::endl;
        std::cout << infoLog << std::endl;
        return;
    }

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success) return;

    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    std::cout << "Error within " << type << std::endl;
    std::cout << infoLog << std::endl;
}
