#include "simple_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace ove {
    // https://docs.vulkan.org/guide/latest/shader_memory_layout.html#alignment-requirements
    struct SimplePushConstantData {
        glm::mat4 modelMatrix{1.0f};
        glm::vec4 color;
        int isLight{0};
    };

    SimpleRenderSystem::SimpleRenderSystem(OveDevice &device, VkRenderPass renderPass,
                                           VkDescriptorSetLayout globalSetLayout) : oveDevice{device} {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(oveDevice.device(), pipelineLayout, nullptr);
    }

    void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange;
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(oveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        OvePipeline::defaultConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        ovePipeline = std::make_unique<OvePipeline>(
                oveDevice,
                "../shaders/simple_shader.vert.spv",
                "../shaders/simple_shader.frag.spv",
                pipelineConfig
        );
    }

    void SimpleRenderSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo) {
        auto camera = frameInfo.scene->camera;
        ubo.projectionView = camera.getProjection() * camera.getTransform();
        auto lightSize = frameInfo.scene->lights.size();
        ubo.numLights = static_cast<int>(lightSize);
        auto rotate = glm::rotate(glm::mat4(1.0f), frameInfo.frameTime, {0.0f, -1.0f, 0.0f});
        for (auto i = 0; i < lightSize; i++) {
            assert(i < MAX_LIGHTS && "lights exceed max number");
            auto &light = frameInfo.scene->lights[i];
            light.transform.translation = glm::vec3(rotate * glm::vec4(light.transform.translation, 1.0f));

            ubo.pointLights[i].position = light.transform.translation;
            ubo.pointLights[i].color = light.color;
        }
    }

    void SimpleRenderSystem::render(FrameInfo &frameInfo) {
        ovePipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipelineLayout, 0, 1,
                                &frameInfo.globalDescriptorSet,
                                0, nullptr);

        for (auto &obj: frameInfo.scene->gameObjects) {
            SimplePushConstantData push{};
            push.modelMatrix = obj.transform.evaluate();
            push.color = obj.color;
            push.isLight = 0;

            vkCmdPushConstants(
                    frameInfo.commandBuffer,
                    pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0, sizeof(SimplePushConstantData),
                    &push);
            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }

        for (auto &light: frameInfo.scene->lights) {
            SimplePushConstantData push{};
            push.modelMatrix = light.transform.evaluate();
            push.color = light.color;
            push.isLight = 1;

            vkCmdPushConstants(
                    frameInfo.commandBuffer,
                    pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0, sizeof(SimplePushConstantData),
                    &push);
            light.model->bind(frameInfo.commandBuffer);
            light.model->draw(frameInfo.commandBuffer);
        }
    }
}