#version 450

layout(location = 0) in vec3 fragWorldPosition;
layout(location = 1) in vec3 fragWorldNormal;

layout(location = 0) out vec4 outColor;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
//    vec3 directionToLight;
    vec4 ambientLight;
    int numLights;
    PointLight lights[10];
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    vec4 color;
    int isLight;
} push;

void main() {
    if (push.isLight == 1) {
        outColor = vec4(push.color.xyz, 1.0);
    } else {
        vec3 diffuseLight = ubo.ambientLight.xyz;
        vec3 surfaceNormal = normalize(fragWorldNormal);
        for (int i = 0; i < ubo.numLights; i++) {
            PointLight light = ubo.lights[i];
            vec3 directionToLight = light.position.xyz - fragWorldPosition;
            float attenuation = 1.0 / dot(directionToLight, directionToLight);  // distance squared
            vec3 lightColor = light.color.xyz * attenuation;
            diffuseLight += lightColor * max(dot(normalize(fragWorldNormal), normalize(directionToLight)), 0);
        }
        outColor = vec4(diffuseLight * push.color.xyz, 1.0);
    }
}