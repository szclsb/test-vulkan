#pragma once

#include "ove_scene.h"

#include <vulkan/vulkan.h>

namespace ove {
    #define MAX_LIGHTS 10

    // https://docs.vulkan.org/guide/latest/shader_memory_layout.html#alignment-requirements
    struct PointLight {
        glm::vec3 position;
        alignas(16) glm::vec3 color;
    };

    struct GlobalUbo {
        glm::mat4 projectionView{1.f};
//        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f});
        glm::vec3 ambientLight{0.2f};
        alignas(16) int numLights;
        alignas(16) PointLight pointLights[MAX_LIGHTS];
    };

    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        VkDescriptorSet globalDescriptorSet;
        std::shared_ptr<Scene> scene;
    };
}