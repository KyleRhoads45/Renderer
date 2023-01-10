#version 460 core

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec3 iTangent;
layout (location = 3) in vec2 iTextureCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 normal;
out vec2 textureCoordinate;
out mat3 tbn;

void main() {
	FragPos = vec3(model * vec4(iPos, 1.0));
	textureCoordinate = iTextureCoordinate;
	//normal = mat3(transpose(inverse(model))) * iNormal;

	normal = normalize(vec3(model * vec4(iNormal, 0.0)));
	vec3 tangent = normalize(vec3(model * vec4(iTangent, 0.0)));
	vec3 bitangent = cross(normal, tangent);
	tbn = mat3(tangent, bitangent, normal);

	gl_Position = projection * view * model * vec4(iPos, 1.0f);
}
