#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragWorldPosition;
layout(location = 2) out vec3 fragWorldNormal;

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

const float AMBIENT = 0.02;

void main() {
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);

    gl_Position = ubo.projectionViewMatrix * positionWorld;

    fragWorldPosition = positionWorld.xyz;
    fragWorldNormal = normalize(mat3(push.modelMatrix) * normal);
    fragColor = color;
}