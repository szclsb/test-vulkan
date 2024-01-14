#pragma once

#include "ove_camera.h"
#include "ove_device.h"
#include "ove_frame_info.h"
#include "ove_game_object.h"
#include "ove_pipeline.h"

#include <memory>
#include <vector>

namespace ove {
    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(OveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void renderGameObjects(FrameInfo &frameInfo, std::vector<OveGameObject> &gameObjects);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        OveDevice &oveDevice;

        std::unique_ptr<OvePipeline> ovePipeline;
        VkPipelineLayout pipelineLayout;
    };
}