#version 460 core

in vec3 fragPos;
in vec3 modelNormal;
in vec2 textureCoord;
in mat3 tbn;
in vec4 lightFragPos;

layout (location = 0) uniform sampler2D albedoMap;
layout (location = 1) uniform sampler2D normalMap;
layout (location = 2) uniform sampler2D metallicRoughnessMap;
layout (location = 3) uniform sampler2D shadowMap;
layout (location = 4) uniform samplerCube skybox;
layout (location = 5) uniform sampler3D shadowPcfMap;

uniform bool albedoMapEnabled;
uniform bool normalMapEnabled;
uniform bool metallicRoughnessMapEnabled;
uniform bool alphaClippingEnabled;
uniform float alphaCutoff;

uniform float roughness;
uniform float specularStrength;
uniform float metallic;


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
    float shadowStrength;
};

out vec4 fragColor;

#define PI 3.1415926538

struct BrdfData {
    vec3 color;
    vec3 normal;
    vec3 halfway;
    vec3 lightDir;
    vec3 viewDir;
    float metallic;
    float roughness;
};

float Sqr(float x) {
    return x * x;
}

vec3 SchlickFresnel(vec3 color, float viewDotHalf, float metallic) {
    vec3 f0 = mix(vec3(0.04f), color, metallic);
    return (f0 + (1.0f - f0) * pow(1.0f - viewDotHalf, 5));
}

float GGX(float normalDotHalf, float roughness) {
    float aSqr = Sqr(Sqr(roughness));
    return aSqr / (PI * Sqr(Sqr(normalDotHalf) * (aSqr - 1.0f) + 1.0f));
}

float GeometrySchlickGGX(float k, float cosTheta) {
    return cosTheta / (cosTheta * (1.0f - k) + k);
}

float SmithGGX(float lightDotHalf, float viewDotHalf, float roughness) {
    float k = Sqr(roughness + 1) / 8.0f;
    float smithL = GeometrySchlickGGX(k, lightDotHalf);
    float smithV = GeometrySchlickGGX(k, viewDotHalf);
    return smithL * smithV;
}

float DisneyDiffuseFresnel(float f90, float theta) {
    float f0 = 1.0f;
    return mix(1.0f, f90, pow(clamp(f0 - theta, 0.0f, 1.0f), 5));
}

vec3 BRDF(BrdfData brdf) {
    float lightDotNormal = max(dot(brdf.lightDir, brdf.normal), 0.0f);
    float viewDotNormal  = max(dot(brdf.viewDir, brdf.normal), 0.0f);
    float lightDotHalf   = max(dot(brdf.lightDir, brdf.halfway), 0.0f);
    float normalDotHalf  = max(dot(brdf.normal, brdf.halfway), 0.0f);
    float viewDotHalf    = max(dot(brdf.viewDir, brdf.halfway), 0.0f);
    
    // Disney Diffuse
    float f90 = 0.5f + 2 * roughness * Sqr(lightDotHalf);
    float fresnelLight = DisneyDiffuseFresnel(f90, lightDotNormal);
    float fresnelView = DisneyDiffuseFresnel(f90, viewDotNormal);
    vec3 diffuse = (brdf.color / PI) * (fresnelLight * fresnelView);
     
    // Metallic
    diffuse *= (1.0f - brdf.metallic);
    
    // Specular
    float specularDist = GGX(normalDotHalf, brdf.roughness);
    float geometricAtt = SmithGGX(lightDotNormal, viewDotNormal, brdf.roughness);
    vec3 fresnel = SchlickFresnel(brdf.color, viewDotHalf, brdf.metallic);
    vec3 specular = specularDist * geometricAtt * fresnel * specularStrength;

    // Prevent the microfacet denominator from being 0
    float microfacetFactor = max(4.0f * lightDotNormal * viewDotNormal, 0.001f);
    return (diffuse + specular / microfacetFactor) * lightDotNormal;
}

float GetShadowBias(float depth) {
    float dx = abs(dFdx(depth));
    float dy = abs(dFdy(depth));
    float slope = max(dx, dy);
    
    const float slopeFactor = 0.1f;
    const float bias = 0.001f;
    return slopeFactor * slope + bias;
}

// Applies randomized disc sampling and returns 0.0f if frag is in shadow, 1.0f otherwise.
float GetShadowValue(ivec3 offsetCoord, vec3 clipPos, vec2 texelSize) {
    vec2 offset = texelFetch(shadowPcfMap, offsetCoord, 0).rg * pcfFilterRadius;
    vec2 shadowCoord = clipPos.xy + (offset * texelSize);
    float shadowMapDepth = texture(shadowMap, shadowCoord).r;
    float bias = GetShadowBias(shadowMapDepth);
    return clipPos.z > shadowMapDepth + bias ? 0.0f : 1.0f;
}

float CalculateShadow() {
    // Perform perspective divide manually to get clip coords
    vec3 clipPos = (lightFragPos / lightFragPos.w).xyz;
    
    // Convert the coordinate from clip space [-1, 1] to the shadow depth map range [0, 1]
    clipPos = clipPos * 0.5f + 0.5f;
    
    // If the clip depth is beyond the shadow map depth, we don't want to shadow the fragment
    if (clipPos.z > 1.0f) {
        return 1.0f;
    }
    
    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
    vec2 offsetCoordYZ = mod(gl_FragCoord.xy, vec2(pcfWindowSize));
    ivec3 offsetCoord = ivec3(0, offsetCoordYZ);
    
    // Sum shadow values around the outermost points of the PCF disc
    for (int i = 0; i < pcfFilterSize; i++) {
        offsetCoord.x = i;
        shadow += GetShadowValue(offsetCoord, clipPos, texelSize);
    }
    
    // Check to see if the outer ring is fully in shadow or fully out of shadow.
    // If either is true, then we can stop processing
    float outerShadowRing = shadow / pcfFilterSize;
    if (outerShadowRing == 0.0f || outerShadowRing == 1.0f) {
        return outerShadowRing;
    }
    
    // Continue PCF'ing the remaining disc sample points 
    int texelsPerFilter = pcfFilterSize * pcfFilterSize;
    for (int i = pcfFilterSize; i < texelsPerFilter; i++) {
        offsetCoord.x = i;
        shadow += GetShadowValue(offsetCoord, clipPos, texelSize);
    }

    return shadow / texelsPerFilter;
}

void main() {
    vec4 albedoColorWithAlpha = vec4(1.0f);
    vec3 albedoColor = vec3(1.0f);
    
    if (albedoMapEnabled) {
        albedoColorWithAlpha = texture(albedoMap, textureCoord);
        albedoColor = albedoColorWithAlpha.rgb;
        if (alphaClippingEnabled && albedoColorWithAlpha.a < alphaCutoff) {
            discard;
        }
    }

    vec3 normal = modelNormal;
    if (normalMapEnabled) {
        normal = normalize(texture(normalMap, textureCoord).rgb * 2.0f - 1.0f);
        normal = normalize(tbn * normal);
    }
    
    float mappedMetallic = metallic;
    float mappedRoughness = roughness;
    if (metallicRoughnessMapEnabled) {
        vec4 metalRoughness = texture(metallicRoughnessMap, textureCoord).rgba;
        mappedMetallic = metalRoughness.b;
        mappedRoughness = metalRoughness.g;
    }
    
    vec3 lightNormal = -lightDir;
    vec3 viewDir = normalize(camPos - fragPos);
    
    BrdfData brdf;
    brdf.color = albedoColor;
    brdf.normal = normal;
    brdf.halfway = normalize(viewDir + lightNormal);
    brdf.lightDir = lightNormal;
    brdf.viewDir = viewDir;
    brdf.metallic = mappedMetallic * metallic;
    brdf.roughness = mappedRoughness * roughness;
    
    vec3 light = lightColor * lightStrength;
    vec3 ambient = ambientColor * albedoColor * ambientStrength;
     
    if (albedoMapEnabled) {
        vec3 finalColor = BRDF(brdf) * light + ambient;
        float shadow = mix(1.0f, CalculateShadow(), shadowStrength);
        fragColor = vec4(finalColor * shadow, albedoColorWithAlpha.a);
    }
    else {
        fragColor = vec4(1.0f); 
    }
    
}