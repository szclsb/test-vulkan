#include "ove_first_app.h"

#include <stdexcept>
#include <array>

namespace ove {
    FirstApp::FirstApp() {
        loadModels();
        createPipelineLayout();
        createPipeline();
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
        std::vector<OveModel::Vertex> vertices {
                {{0.0f, -0.5f}},
                {{0.5f, 0.5f}},
                {{-0.5f, 0.5f}}
        };

        oveModel = std::make_unique<OveModel>(oveDevice, vertices);
    }

    void FirstApp::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(oveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void FirstApp::createPipeline() {
        auto pipelineConfig = OvePipeline::defaultConfigInfo(oveSwapChain.width(), oveSwapChain.height());
        pipelineConfig.renderPass = oveSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        ovePipeline = std::make_unique<OvePipeline>(
                oveDevice,
                "../shaders/simple_shader.vert.spv",
                "../shaders/simple_shader.frag.spv",
                pipelineConfig
        );
    }

    void FirstApp::createCommandBuffers() {
        commandBuffers.resize(oveSwapChain.imageCount());

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = oveDevice.getCommandPool();
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(oveDevice.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers");
        }

        for (auto i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer");
            }

            VkRenderPassBeginInfo renderBeginInfo{};
            renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderBeginInfo.renderPass = oveSwapChain.getRenderPass();
            renderBeginInfo.framebuffer = oveSwapChain.getFrameBuffer(i);

            renderBeginInfo.renderArea.offset = {0, 0};
            renderBeginInfo.renderArea.extent = oveSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            // clearValues[0].depthStencil would be ignored because we are using 2 framebuffer attachments, depends on pipeline layout
            clearValues[1].depthStencil = {1.0f, 0};
            renderBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderBeginInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            ovePipeline->bind(commandBuffers[i]);
            oveModel->bind(commandBuffers[i]);
            oveModel->draw(commandBuffers[i]);

            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer");
            }
        }
    }

    void FirstApp::drawFrame() {
        uint32_t imageIndex;
        auto result = oveSwapChain.acquireNextImage(&imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        result = oveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }
    }
}