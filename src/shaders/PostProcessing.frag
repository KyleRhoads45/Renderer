#version 460 core

in vec2 texCoord;

layout (location = 0) uniform sampler2D hdrTexture;

uniform float contrast;
uniform float saturation;
uniform float exposure;

out vec4 fragColor;

// Map color values back into the range [0, 1]
vec3 Aces(vec3 color) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

vec3 Contrast(vec3 color, float value) {
    return (color - 0.5f) * (1.0f + value) + 0.5f;
}

vec3 Saturation(vec3 color, float value) {
    const vec3 luminosityFactor = vec3(0.2126f, 0.7152f, 0.0722f);
    vec3 grayscale = vec3(dot(color, luminosityFactor));
    return mix(grayscale, color, 1.0f + value);
}

vec3 Exposure(vec3 color, float value) {
    return color * (value + 1.0f);
}

void main() {
    vec3 hdrColor = texture(hdrTexture, texCoord).rgb;
    vec3 finalColor = Aces(hdrColor);
    finalColor = Contrast(finalColor, contrast);
    finalColor = Saturation(finalColor, saturation);
    finalColor = Exposure(finalColor, exposure);
    fragColor = vec4(finalColor, 1.0);
}