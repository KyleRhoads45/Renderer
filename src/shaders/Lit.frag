#version 460 core

in vec3 fragPos;
in vec3 normal;
in vec2 textureCoord;

uniform float shininess;
uniform float specularStrength;
uniform sampler2D albedo;

layout (std140, binding = 0) uniform camera {
	vec3 camPos;
	mat4 view;
	mat4 projection;
	mat4 viewProjection;
};

layout (std140, binding = 1) uniform enviorment {
    float ambientStrength;
    vec3 ambientColor;
};

out vec4 fragColor;

vec3 GetLightColor();
vec3 GetSpecularColor();

void main() {
    vec3 diffuseColor = GetLightColor();
    vec3 specularColor = GetSpecularColor();

    vec3 ambientColor = ambientColor * ambientStrength;
    vec3 albedoColor = texture(albedo, textureCoord).rgb;

    fragColor = vec4(albedoColor * (diffuseColor + ambientColor + specularColor), 1.0);
}

vec3 GetLightColor() {
    vec3 lightPos = vec3(10.0, 10.0, 10.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0) * 2.0;
    vec3 lightDir = normalize(lightPos - fragPos);

    float diffuseLightStrength = max(dot(normal, lightDir), 0.0);
    return lightColor * diffuseLightStrength;
}

vec3 GetSpecularColor() {
    vec3 lightPos = vec3(10.0, 10.0, 10.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0) * 2.0;
    vec3 lightDir = normalize(lightPos - fragPos);

    vec3 viewDir = normalize(camPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    return specularStrength * spec * lightColor; 
}