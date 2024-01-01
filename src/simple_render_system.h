#pragma once

#include "ove_pipeline.h"
#include "ove_device.h"
#include "ove_game_object.h"
#include "ove_camera.h"

#include <memory>
#include <vector>

namespace ove {
    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(OveDevice &device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<OveGameObject> &gameObjects, const OveCamera &camera);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        OveDevice &oveDevice;

        std::unique_ptr<OvePipeline> ovePipeline;
        VkPipelineLayout pipelineLayout;
    };
}