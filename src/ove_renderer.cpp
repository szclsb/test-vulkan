#include "ove_renderer.h"

#include <stdexcept>
#include <array>

namespace ove {
    OveRenderer::OveRenderer(OveWindow &window, OveDevice &device) : oveWindow{window},
                                                                     oveDevice{device},
                                                                     isFrameStarted{false},
                                                                     currentFrameIndex{0} {
        recreateSwapChain();
        createCommandBuffers();
    }

    OveRenderer::~OveRenderer() {
        freeCommandBuffers();
    }

    void OveRenderer::createCommandBuffers() {
        commandBuffers.resize(OveSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = oveDevice.getCommandPool();
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(oveDevice.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void OveRenderer::freeCommandBuffers() {
        vkFreeCommandBuffers(
                oveDevice.device(),
                oveDevice.getCommandPool(),
                static_cast<uint32_t>(commandBuffers.size()),
                commandBuffers.data());
        commandBuffers.clear();
    }

    void OveRenderer::recreateSwapChain() {
        auto extent = oveWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = oveWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(oveDevice.device());

        if (oveSwapChain == nullptr) {
            oveSwapChain = std::make_unique<OveSwapChain>(oveDevice, extent);
        } else {
            std::shared_ptr<OveSwapChain> oldSwapChain = std::move(oveSwapChain);
            oveSwapChain = std::make_unique<OveSwapChain>(oveDevice, extent, oldSwapChain);
            if (!oldSwapChain->compareSwapFormats(*oldSwapChain)) {
                throw std::runtime_error("swap chain image (or depth) format has changed!");
            }
        }
        //fixme
        //createPipeline();
    }

    VkCommandBuffer OveRenderer::beginFrame() {
        assert(!isFrameStarted && "cant call beginFrame while already in progress");
        auto result = oveSwapChain->acquireNextImage(&currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer");
        }
        return commandBuffer;
    }

    void OveRenderer::endFrame() {
        assert(isFrameStarted && "cant call end frame while frame is not in progress");

        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer");
        }

        auto result = oveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || oveWindow.wasResized()) {
            oveWindow.resetResizeFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % OveSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void OveRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "cant call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() &&
               "cant begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderBeginInfo{};
        renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderBeginInfo.renderPass = oveSwapChain->getRenderPass();
        renderBeginInfo.framebuffer = oveSwapChain->getFrameBuffer(currentImageIndex);

        renderBeginInfo.renderArea.offset = {0, 0};
        renderBeginInfo.renderArea.extent = oveSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        // clearValues[0].depthStencil would be ignored because we are using 2 framebuffer attachments, depends on pipeline layout
        clearValues[1].depthStencil = {1.0f, 0};
        renderBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float >(oveSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float >(oveSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, oveSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    }

    void OveRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "cant call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() &&
               "cant end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }


}