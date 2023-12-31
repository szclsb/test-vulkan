#include "ove_first_app.h"

#include "simple_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace ove {
    FirstApp::FirstApp() {
        loadGameObjects();
    }

    FirstApp::~FirstApp() {
    }

    void FirstApp::loadGameObjects() {
        std::vector<OveModel::Vertex> vertices{
                {{0.0f,  -0.5f}},
                {{0.5f,  0.5f}},
                {{-0.5f, 0.5f}}
        };

        auto model = std::make_shared<OveModel>(oveDevice, vertices);

        auto triangle = OveGameObject::createGameObject();
        triangle.model = model;
        triangle.color = {0.1f, 0.8f, 0.1f};
        triangle.transform2d.translation = {0.2f, 0.0f};
        triangle.transform2d.scale = {2.0f, 0.5f};
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }

    void FirstApp::run() {
        SimpleRenderSystem simpleRenderSystem{oveDevice, oveRenderer.getSwapChainRenderPass()};

        while (!oveWindow.shouldClose()) {
            glfwPollEvents();

            if (auto commandBuffer = oveRenderer.beginFrame()) {
                oveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                oveRenderer.endSwapChainRenderPass(commandBuffer);
                oveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(oveDevice.device());
    }
}