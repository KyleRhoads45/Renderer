#version 460 core

in vec3 fragPos;
in vec3 modelNormal;
in vec2 textureCoord;
in mat3 tbn;
in vec4 lightFragPos;

layout (binding = 0) uniform sampler2D baseMap;
layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform sampler2D shadowMap;
layout (binding = 3) uniform samplerCube skybox;

uniform bool useDiffuse;
uniform bool useNormal;
uniform bool alphaClippingEnabled;
uniform float alphaCutoff;

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

out vec4 fragColor;

#define PI 3.1415926538

float CalculateShadow(vec3 normal, vec3 lightNormal);
vec3 CalculateSpecular(vec3 baseColor, vec3 lightNormal, vec3 normal, float metallic, float roughness);
vec3 ACESFilm(in vec3 x);
vec3 FrensnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

void main() {
    vec4 baseColorWithAlpha = vec4(1.0);
    vec3 baseColor = vec3(1.0);
    
    if (useDiffuse) {
        baseColorWithAlpha = texture(baseMap, textureCoord);
        baseColor = baseColorWithAlpha.rgb;
        if (alphaClippingEnabled && baseColorWithAlpha.a < alphaCutoff) {
            discard;
        }
    }

    vec3 normal = modelNormal;
    if (useNormal) {
        normal = texture(normalMap, textureCoord).rgb;
        normal = normalize((normal * 2.0) - 1.0);
        normal = normalize(tbn * normal);
    }

    vec3 lightNormal = -lightDir;
    vec3 totalRadiance = vec3(0.0);
    vec3 diffuse;

    for (int i = 0; i < 1; i++) {
        // Directional light radiance is just light color 
        vec3 radiance = vec3(1.0, 1.0, 1.0) * lightStrength;

        float metallic = 0.0;
        float roughness = 0.5;

        vec3 specular = CalculateSpecular(baseColor, lightNormal, normal, metallic, roughness);
        diffuse = vec3(1.0) - specular;
       
        diffuse = diffuse * (1.0 - metallic);

        float NdotL = max(dot(normal, lightNormal), 0.0);
        totalRadiance += (diffuse * baseColor / PI + specular) * radiance * NdotL;
    }

    vec3 ambientColor = texture(skybox, normal).rgb * baseColor * diffuse * ambientStrength;
    //vec3 finalColor = totalRadiance * CalculateShadow(normal, lightNormal) + ambientColor;
    vec3 finalColor = totalRadiance + ambientColor;
    fragColor = vec4(finalColor, baseColorWithAlpha.a);
}

float CalculateShadow(vec3 normal, vec3 lightNormal) {
    // Transform from clip space to device coords [-1, 1]
    vec3 projCoord = lightFragPos.xyz / lightFragPos.w;

    // Transform to [0, 1] for depth map sampling
    projCoord = projCoord * 0.5 + 0.5;

    // Prevent fragments from being black that are outside of the shadowMap's range
    if (projCoord.z > 1.0 || projCoord.z < 0.0) return 1.0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
            vec2 coord = projCoord.xy + (vec2(x, y) * texelSize);
			float shadowDepth = texture(shadowMap, coord).r;
			float fragDepth = projCoord.z;

			float bias = 0.00012;
			shadow += (fragDepth > shadowDepth + bias) ? 0.0 : 1.0;
		}
    }

    return shadow / 9.0;
}

vec3 CalculateSpecular(vec3 baseColor, vec3 lightNormal, vec3 normal, float metallic, float roughness) {
    vec3 viewDir = normalize(camPos - fragPos);
    vec3 halfWay = normalize(viewDir + lightNormal);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, baseColor, metallic);

	float D = DistributionGGX(normal, halfWay, roughness);
	float G = GeometrySmith(normal, viewDir, lightNormal, roughness);
    vec3 F = FrensnelSchlick(max(dot(halfWay, viewDir), 0.0), F0);

    return (D * F * G) / (4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightNormal), 0.0) + 0.0001);
}

vec3 FrensnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 ACESFilm(in vec3 x) {
	float a = 2.51;
	float b = 0.03;
	float c = 2.43;
	float d = 0.59;
	float e = 0.14;
	return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}
