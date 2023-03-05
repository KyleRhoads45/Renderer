#version 460 core

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec2 iTextureCoord;

layout (std140, binding = 0) uniform camera {
	vec3 camPos;
	mat4 view;
	mat4 projection;
	mat4 viewProjection;
};

uniform mat4 model;

out vec3 fragPos;
out vec3 normal;
out vec2 textureCoord;

void main() {
	textureCoord = iTextureCoord;
	fragPos = vec3(model * vec4(iPos, 1.0));
	normal = mat3(transpose(inverse(model))) * iNormal;
	gl_Position = viewProjection * model * vec4(iPos, 1.0);
}
