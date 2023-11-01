#pragma once

#include "ove_window.h"
#include "ove_pipeline.h"
#include "ove_device.h"
#include "ove_swap_chain.h"

#include <memory>
#include <vector>

namespace ove {
    class FirstApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp &) = delete;
        FirstApp &operator=(const FirstApp &) = delete;

        void run();

    private:
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();

        OveWindow oveWindow{WIDTH, HEIGHT, "Hello Vulkan"};
        OveDevice oveDevice{oveWindow};
        OveSwapChain oveSwapChain{oveDevice, oveWindow.getExtent()};
        std::unique_ptr<OvePipeline> ovePipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
    };
}