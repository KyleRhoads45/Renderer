#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "Shader.h"

Shader::Shader() {
    CreateShader("src/shaders/Lit.vert", "src/shaders/Lit.frag");
}

Shader::Shader(const std::string& vertFile, const std::string& fragFile) {
    CreateShader(vertFile, fragFile);
}

void Shader::Bind() const {
    glUseProgram(shaderId);
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4& mat4) const {
    glUniformMatrix4fv(glGetUniformLocation(shaderId, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat4));
}

void Shader::CreateShader(const std::string& vertFile, const std::string& fragFile) {
    const std::string vertCodeString = LoadShaderFile(vertFile);
    const std::string fragCodeString = LoadShaderFile(fragFile);

    const char* vertCode = vertCodeString.c_str();
    const char* fragCode = fragCodeString.c_str();

    //Vertex shader
    const unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertCode, NULL);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "Vertex");

    //Fragment Shader
    const unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragCode, NULL);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "Fragment");

    //Shader Program
    shaderId = glCreateProgram();
    glAttachShader(shaderId, vertex);
    glAttachShader(shaderId, fragment);
    glLinkProgram(shaderId);
    CheckCompileErrors(shaderId, "Shader Linking");

    //Delete the shaders as they're linked into our program and are no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    Bind();
}

//Loads a shader file into a string
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

//Checks for compilation errors in the shaders
void Shader::CheckCompileErrors(const unsigned int shader, const std::string& type) const {
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
