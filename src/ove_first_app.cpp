#include "ove_first_app.h"

#include "simple_render_system.h"
#include "keyboard_movement_controller.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <chrono>

namespace ove {
    struct GlobalUbo {
        glm::mat4 projectionView{1.f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f});
    };

    FirstApp::FirstApp() {
        globalPool = OveDescriptorPool::Builder(oveDevice)
                .setMaxSets(OveSwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, OveSwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();
        loadGameObjects();
    }

    FirstApp::~FirstApp() {
    }

    void FirstApp::loadGameObjects() {
        std::shared_ptr<OveModel> model = OveModel::createModelFromFile(oveDevice, "../models/smooth_vase.obj");
        auto cube = OveGameObject::createGameObject();
        cube.model = model;
//        cube.color = {0.1f, 0.8f, 0.1f};
        cube.transform.translation = {0.0f, 0.0f, 2.5f};
        cube.transform.scale = 0.5f;

        gameObjects.push_back(std::move(cube));
    }

    void FirstApp::run() {
        std::vector<std::unique_ptr<OveBuffer>> uboBuffers(OveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<OveBuffer>(
                    oveDevice,
                    sizeof(GlobalUbo),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        auto globalSetLayout = OveDescriptorSetLayout::Builder(oveDevice)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                .build();
        std::vector<VkDescriptorSet> globalDescriptorSets(OveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (auto i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            OveDescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{oveDevice,
                                              oveRenderer.getSwapChainRenderPass(),
                                              globalSetLayout->getDescriptorSetLayout()};
        OveCamera camera{};
        camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.2f, 0.0f, 1.0f));

        KeyboardMovementController controller{};
        auto viewerObject = OveGameObject::createGameObject();

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!oveWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            controller.moveInPlaneXZ(oveWindow.getWindowPtr(), frameTime, viewerObject);
            camera.setViewXyz(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = oveRenderer.getAspectRatio();
//            camera.setOrthographicProjection(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.01f, 10.0f);

            if (auto commandBuffer = oveRenderer.beginFrame()) {
                int frameIndex = oveRenderer.getFrameIndex();
                FrameInfo frameInfo{
                        frameIndex,
                        frameTime,
                        commandBuffer,
                        globalDescriptorSets[frameIndex],
                        camera
                };

                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getTransform();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                oveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                oveRenderer.endSwapChainRenderPass(commandBuffer);
                oveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(oveDevice.device());
    }
}