#version 460 core

in vec3 FragPos;
in vec3 normal;
in vec2 textureCoordinate;
in mat3 tbn;

uniform sampler2D albedo;
uniform sampler2D normalMap;

out vec4 FragColor;

void main() {
    vec3 lightPos = vec3(10.0, 10.0, 10.0);
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f) * 2.0;
    vec3 lightDir = normalize(lightPos - FragPos);

    vec3 nMapNormal = texture(normalMap, textureCoordinate).rgb;
    nMapNormal = (nMapNormal * 2.0) - 1.0;
    nMapNormal = normalize(tbn * nMapNormal);
    
    vec3 norm = normalize(normal);
    
    float diffuseStrength = max(dot(nMapNormal, lightDir), 0.0);
    vec3 diffuseColor = lightColor * diffuseStrength;
    vec3 ambientColor = lightColor * 0.1;
    
    FragColor = texture(albedo, textureCoordinate) * vec4(ambientColor + diffuseColor, 1.0);
}