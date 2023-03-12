#version 460 core

in vec2 texCoord;

layout (location = 4) uniform sampler2D baseMap;

out vec4 fragColor; 

void main() {
	fragColor = vec4(vec3(texture(baseMap, texCoord).r), 1.0);
}
