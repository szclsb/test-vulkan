#include "ove_first_app.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

#include <stdexcept>
#include <array>

namespace ove {
    // https://docs.vulkan.org/guide/latest/shader_memory_layout.html#alignment-requirements
    struct SimplePushConstantData {
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    FirstApp::FirstApp() {
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(oveDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run() {
        while (!oveWindow.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(oveDevice.device());
    }

    void FirstApp::loadModels() {
        std::vector<OveModel::Vertex> vertices{
                {{0.0f,  -0.5f}},
                {{0.5f,  0.5f}},
                {{-0.5f, 0.5f}}
        };

        oveModel = std::make_unique<OveModel>(oveDevice, vertices);
    }

    void FirstApp::createPipelineLayout() {
        VkPushConstantRange pushConstantRange;
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(oveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void FirstApp::createPipeline() {
        assert(oveSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        OvePipeline::defaultConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = oveSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        ovePipeline = std::make_unique<OvePipeline>(
                oveDevice,
                "../shaders/simple_shader.vert.spv",
                "../shaders/simple_shader.frag.spv",
                pipelineConfig
        );
    }

    void FirstApp::createCommandBuffers() {
        commandBuffers.resize(oveSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = oveDevice.getCommandPool();
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(oveDevice.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void FirstApp::freeCommandBuffers() {
        vkFreeCommandBuffers(
                oveDevice.device(),
                oveDevice.getCommandPool(),
                static_cast<uint32_t>(commandBuffers.size()),
                commandBuffers.data());
        commandBuffers.clear();
    }

    void FirstApp::recordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo renderBeginInfo{};
        renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderBeginInfo.renderPass = oveSwapChain->getRenderPass();
        renderBeginInfo.framebuffer = oveSwapChain->getFrameBuffer(imageIndex);

        renderBeginInfo.renderArea.offset = {0, 0};
        renderBeginInfo.renderArea.extent = oveSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        // clearValues[0].depthStencil would be ignored because we are using 2 framebuffer attachments, depends on pipeline layout
        clearValues[1].depthStencil = {1.0f, 0};
        renderBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float >(oveSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float >(oveSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, oveSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        ovePipeline->bind(commandBuffers[imageIndex]);
        oveModel->bind(commandBuffers[imageIndex]);

        for (int j = 0; j < 4; j++) {
            SimplePushConstantData push{};
            push.offset = {0.0f, -0.4f + j * 0.25f};
            push.color = {0.0f, 0.0f, 0.2f + j * 0.2f};

            vkCmdPushConstants(
                    commandBuffers[imageIndex],
                    pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0, sizeof(SimplePushConstantData),
                    &push);
            oveModel->draw(commandBuffers[imageIndex]);
        }

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer");
        }
    }

    void FirstApp::recreateSwapChain() {
        auto extent = oveWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = oveWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(oveDevice.device());

        if (oveSwapChain == nullptr) {
            oveSwapChain = std::make_unique<OveSwapChain>(oveDevice, extent);
        } else {
            oveSwapChain = std::make_unique<OveSwapChain>(oveDevice, extent, std::move(oveSwapChain));
            if (oveSwapChain->imageCount() != commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }
        createPipeline();
    }

    void FirstApp::drawFrame() {
        uint32_t imageIndex;
        auto result = oveSwapChain->acquireNextImage(&imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        recordCommandBuffer(imageIndex);
        result = oveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || oveWindow.wasResized()) {
            oveWindow.resetResizeFlag();
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }
    }
}