#pragma once

#include "ove_camera.h"

#include <vulkan/vulkan.h>

namespace ove {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        VkDescriptorSet globalDescriptorSet;
        OveCamera camera;
    };
}