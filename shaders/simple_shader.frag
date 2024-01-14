#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragWorldPosition;
layout(location = 2) in vec3 fragWorldNormal;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
//    vec3 directionToLight;
    vec3 ambientLight;
    vec3 lightPosition;
    vec3 lightColor;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
} push;

void main() {
    vec3 directionToLight = ubo.lightPosition - fragWorldPosition;
    float attenuation = 1.0 / dot(directionToLight, directionToLight);  // distance squared
    vec3 lightColor = ubo.lightColor * attenuation;
    vec3 diffuseLight = lightColor * max(dot(normalize(fragWorldNormal), normalize(directionToLight)), 0);

    outColor = vec4((ubo.ambientLight + diffuseLight) * fragColor, 1.0);
}