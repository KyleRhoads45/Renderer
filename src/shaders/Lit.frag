#version 460 core

in vec3 fragPos;
in vec3 normal;
in vec2 textureCoord;

uniform float metallic;
uniform float roughness;
uniform sampler2D baseMap;

uniform samplerCube skybox;

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

#define PI 3.1415926538

vec3 CalculateSpecular(vec3 baseColor, vec3 lightDir);
vec3 ACESFilm(in vec3 x);
vec3 FrensnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

void main() {
    vec3 lightDir = vec3(-0.33, -0.33, 0.33);
    vec3 totalRadiance = vec3(0.0);
    vec3 diffuse;

	vec3 baseColor = texture(baseMap, textureCoord).rgb;
	baseColor.r = pow(baseColor.r, 2.2);
	baseColor.g = pow(baseColor.g, 2.2);
	baseColor.b = pow(baseColor.b, 2.2);

    for (int i = 0; i < 1; i++) {
        // Directional light radiance is just light color 
        vec3 radiance = vec3(1.0, 1.0, 1.0) * 11.0;

        vec3 specular = CalculateSpecular(baseColor, lightDir);
        diffuse = vec3(1.0) - specular;

        diffuse = diffuse * (1.0 - metallic);

        float NdotL = max(dot(normal, lightDir), 0.0);
        totalRadiance += (diffuse * baseColor / PI + specular) * radiance * NdotL;
    }

    vec3 ambientColor = texture(skybox, normal).rgb * baseColor * diffuse * 1.0;
    vec3 finalColor = totalRadiance + ambientColor;

    float contrast = 1.1;
    finalColor = ((finalColor - 0.5) * max(contrast, 0.0)) + 0.5;

    fragColor = vec4(ACESFilm(finalColor), 1.0);
}

vec3 CalculateSpecular(vec3 baseColor, vec3 lightDir) {
    vec3 viewDir = normalize(camPos - fragPos);
    vec3 halfWay = normalize(viewDir + lightDir);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, baseColor, metallic);

	float D = DistributionGGX(normal, halfWay, roughness);
	float G = GeometrySmith(normal, viewDir, lightDir, roughness);
    vec3 F = FrensnelSchlick(max(dot(halfWay, viewDir), 0.0), F0);

    return (D * F * G) / (4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001);
}

vec3 FrensnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 ACESFilm(in vec3 x) {
	float a = 2.51;
	float b = 0.03;
	float c = 2.43;
	float d = 0.59;
	float e = 0.14;
	return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}