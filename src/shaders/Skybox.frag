#version 460 core

in vec3 texCoords;

uniform samplerCube cubeMap;

out vec4 FragColor;

void main() {
    FragColor = texture(cubeMap, texCoords);
}
