#version 460 core

in vec2 texCoords;

uniform sampler2D depthMap;

out vec4 fragColor;

void main() {
    fragColor = vec4(vec3(texture(depthMap, texCoords).r), 1.0f);
}