#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec3 fragWorldPosition;
layout(location = 1) out vec3 fragWorldNormal;

struct PointLight {
    vec3 position;
    vec3 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
//    vec4 directionToLight;
    vec3 ambientLight;
    int numLights;
    PointLight lights[10];
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    vec4 color;
    int isLight;
} push;

void main() {
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);

    gl_Position = ubo.projectionViewMatrix * positionWorld;

    fragWorldPosition = positionWorld.xyz;
    fragWorldNormal = normalize(mat3(push.modelMatrix) * normal);
}