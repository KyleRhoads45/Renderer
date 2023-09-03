#version 460 core

layout (location = 0) in vec3 iPos;

layout (std140, binding = 0) uniform camera {
	vec3 camPos;
	mat4 view;
	mat4 projection;
	mat4 viewProjection;
};

uniform mat4 model;

void main() {
	gl_Position = viewProjection * model * vec4(iPos, 1.0);
}
