#version 460 core

layout (location = 0) in vec3 iPos;
layout (location = 3) in vec2 iTexCoord;

out vec2 texCoord;

void main() {
	texCoord = iTexCoord;
	gl_Position = vec4(iPos, 1.0);
}
