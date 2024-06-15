#version 460 core

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec3 iTangent;
layout (location = 3) in vec2 iTextureCoord;

layout (std140, binding = 0) uniform camera {
	vec3 camPos;
	mat4 view;
	mat4 projection;
	mat4 viewProjection;
};

layout (std140, binding = 1) uniform enviorment {
    float ambientStrength;
    float lightStrength;
	vec3 lightDir;
    mat4 lightViewProjection;
};

uniform mat4 model;

out vec3 fragPos;
out vec3 modelNormal;
out vec2 textureCoord;
out mat3 tbn;
out vec4 lightFragPos;

void main() {
	textureCoord = iTextureCoord;
	fragPos = vec3(model * vec4(iPos, 1.0));
	lightFragPos = lightViewProjection * vec4(fragPos, 1.0);
	modelNormal = normalize(mat3(transpose(inverse(model))) * iNormal);

	// Calculate tbn matrix
	vec3 tangent = normalize(vec3(model * vec4(iTangent, 0.0)));
	vec3 normal = normalize(vec3(model * vec4(iNormal, 0.0)));
	vec3 biTangent = cross(normal, tangent); 
	tbn = mat3(tangent, biTangent, normal);

	gl_Position = viewProjection * model * vec4(iPos, 1.0);
}
