#version 460 core

in vec2 texCoord;

layout (location = 0) uniform sampler2D baseMap;

out vec4 fragColor; 

void main() {
//	fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	fragColor = vec4(vec3(texture(baseMap, texCoord).r), 1.0);
}
