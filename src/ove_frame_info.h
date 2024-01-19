#pragma once

#include "ove_camera.h"

#include <vulkan/vulkan.h>

namespace ove {
    #define MAX_LIGHTS 10

    struct PointLight {
        glm::vec4 position;
        glm::vec4 color;
    };

    struct GlobalUbo {
        glm::mat4 projectionView{1.f};
//        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f});
        glm::vec4 ambientLight{0.2f};
        int numLights;
        alignas(16) PointLight pointLights[MAX_LIGHTS];
    };

    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        VkDescriptorSet globalDescriptorSet;
        OveCamera camera;
    };
}