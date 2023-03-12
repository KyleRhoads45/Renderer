#version 460 core

layout (location = 0) in vec3 iPos;

uniform mat4 model;
uniform mat4 viewProjection;

void main() {
	gl_Position = viewProjection * model * vec4(iPos, 1.0);
}
