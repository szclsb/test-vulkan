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
    FirstApp::FirstApp() {
        globalPool = OveDescriptorPool::Builder(oveDevice)
                .setMaxSets(OveSwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, OveSwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();
        scene = std::make_shared<Scene>();
        loadGameObjects();
        loadLights();
    }

    FirstApp::~FirstApp() {
    }

    void FirstApp::loadGameObjects() {
        std::shared_ptr<OveModel> flatVase = OveModel::createModelFromFile(oveDevice, "../models/flat_vase.obj");
        std::shared_ptr<OveModel> smoothVase = OveModel::createModelFromFile(oveDevice, "../models/smooth_vase.obj");
        std::shared_ptr<OveModel> floor = OveModel::createModelFromFile(oveDevice, "../models/square.obj");

        auto obj1 = OveGameObject::createGameObject();
        obj1.model = flatVase;
        obj1.transform.translation = {-0.5f, 0.5f, 0};
        obj1.transform.scale = 2.0f;
        scene->gameObjects.push_back(std::move(obj1));

        auto obj2 = OveGameObject::createGameObject();
        obj2.model = smoothVase;
        obj2.transform.translation = {0.5f, 0.5f, 0.0f};
        obj2.transform.scale = 2.0f;
        scene->gameObjects.push_back(std::move(obj2));

        auto obj3 = OveGameObject::createGameObject();
        obj3.model = floor;
        obj3.transform.translation = {0.0f, 0.5f, 0.0f};
        obj3.transform.scale = 5.0f;
        scene->gameObjects.push_back(std::move(obj3));
    }

    void FirstApp::loadLights() {
        std::shared_ptr<OveModel> sphere = OveModel::createModelFromFile(oveDevice, "../models/sphere.obj");
        std::vector<glm::vec3> lightColors{
                {1.f, .1f, .1f},
                {.1f, .1f, 1.f},
                {.1f, 1.f, .1f},
                {1.f, 1.f, .1f},
                {.1f, 1.f, 1.f},
                {1.f, 1.f, 1.f}  //
        };
        for (int i = 0; i < lightColors.size(); i++) {
            auto pointLight = OveGameObject::createGameObject();
            pointLight.color = glm::vec4(lightColors[i], 1.0f);
            pointLight.model = sphere;
            pointLight.transform.scale = 0.1f;
            auto rotateLight = glm::rotate(
                    glm::mat4(1.f),
                    (i * glm::two_pi<float>()) / lightColors.size(),
                    {0.f, -1.f, 0.f});
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
            scene->lights.push_back(std::move(pointLight));
        }
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
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
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
        scene->camera = OveCamera{};
        auto viewerObject = OveGameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController controller{};
//        camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.2f, 0.0f, 1.0f));

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!oveWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            controller.moveInPlaneXZ(oveWindow.getWindowPtr(), frameTime, viewerObject);
            scene->camera.setViewXyz(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = oveRenderer.getAspectRatio();
//            scene->camera.setOrthographicProjection(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
            scene->camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.01f, 100.0f);

            if (auto commandBuffer = oveRenderer.beginFrame()) {
                int frameIndex = oveRenderer.getFrameIndex();
                FrameInfo frameInfo{
                        frameIndex,
                        frameTime,
                        commandBuffer,
                        globalDescriptorSets[frameIndex],
                        scene
                };

                // update
                GlobalUbo ubo{};
                simpleRenderSystem.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                oveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.render(frameInfo);
                oveRenderer.endSwapChainRenderPass(commandBuffer);
                oveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(oveDevice.device());
    }
}