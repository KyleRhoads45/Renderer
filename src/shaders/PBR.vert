#version 460 core

layout(location = 0) in vec3 iPos;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec3 iTangent;
layout(location = 3) in vec2 iTextureCoord;

layout (std140, binding = 0) uniform camera {
	vec3 camPos;
	mat4 view;
	mat4 projection;
	mat4 viewProjection;
};

layout (std140, binding = 1) uniform enviorment {
    vec3 lightColor;
    vec3 ambientColor;
    float ambientStrength;
    float lightStrength;
    vec3 lightDir;
    mat4 lightViewProjection;
    int pcfWindowSize;
    int pcfFilterSize;
    float pcfFilterRadius;
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
	mat3 normalMatrix = mat3(model);
	vec3 normal = normalize(normalMatrix * iNormal);
	vec3 tangent = normalize(normalMatrix * iTangent);
	vec3 bitangent = normalize(cross(normal, tangent));
	tbn = mat3(tangent, bitangent, normal);
	
	gl_Position = viewProjection * model * vec4(iPos, 1.0);
}
