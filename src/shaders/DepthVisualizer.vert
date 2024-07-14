#version 460 core

layout(location = 0) in vec3 iPos;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec3 iTangent;
layout(location = 3) in vec2 iTextureCoord;

out vec2 texCoords;

void main() {
    texCoords = iTextureCoord;
    gl_Position = vec4(iPos, 1.0);
}
