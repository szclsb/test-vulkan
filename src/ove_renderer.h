#pragma once

#include "ove_window.h"
#include "ove_pipeline.h"
#include "ove_device.h"
#include "ove_swap_chain.h"
#include "ove_game_object.h"

#include <cassert>
#include <memory>
#include <vector>

namespace ove {
    class OveRenderer {
    public:
        OveRenderer(OveWindow &window, OveDevice &device);
        ~OveRenderer();

        OveRenderer(const OveRenderer &) = delete;
        OveRenderer &operator=(const OveRenderer &) = delete;

        VkRenderPass getSwapChainRenderPass() const { return oveSwapChain->getRenderPass(); }
        bool isFrameInProgress() const { return isFrameStarted; }
        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get framebuffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }
        int getFrameIndex() const {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }


        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        OveWindow &oveWindow;
        OveDevice &oveDevice;
        std::unique_ptr<OveSwapChain> oveSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex;
        bool isFrameStarted;
    };
}