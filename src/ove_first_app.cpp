#include "ove_first_app.h"

#include "simple_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <chrono>

namespace ove {
    FirstApp::FirstApp() {
        loadGameObjects();
    }

    FirstApp::~FirstApp() {
    }

    void FirstApp::loadGameObjects() {
        std::vector<OveModel::Vertex> vertices{
                // left face (white)
                {{-.5f, -.5f, -.5f},  {.9f, .9f, .9f}},
                {{-.5f, .5f,  .5f},   {.9f, .9f, .9f}},
                {{-.5f, -.5f, .5f},   {.9f, .9f, .9f}},
                {{-.5f, -.5f, -.5f},  {.9f, .9f, .9f}},
                {{-.5f, .5f,  -.5f},  {.9f, .9f, .9f}},
                {{-.5f, .5f,  .5f},   {.9f, .9f, .9f}},

                // right face (yellow)
                {{.5f,  -.5f, -.5f},  {.8f, .8f, .1f}},
                {{.5f,  .5f,  .5f},   {.8f, .8f, .1f}},
                {{.5f,  -.5f, .5f},   {.8f, .8f, .1f}},
                {{.5f,  -.5f, -.5f},  {.8f, .8f, .1f}},
                {{.5f,  .5f,  -.5f},  {.8f, .8f, .1f}},
                {{.5f,  .5f,  .5f},   {.8f, .8f, .1f}},

                // top face (orange, remember y axis points down)
                {{-.5f, -.5f, -.5f},  {.9f, .6f, .1f}},
                {{.5f,  -.5f, .5f},   {.9f, .6f, .1f}},
                {{-.5f, -.5f, .5f},   {.9f, .6f, .1f}},
                {{-.5f, -.5f, -.5f},  {.9f, .6f, .1f}},
                {{.5f,  -.5f, -.5f},  {.9f, .6f, .1f}},
                {{.5f,  -.5f, .5f},   {.9f, .6f, .1f}},

                // bottom face (red)
                {{-.5f, .5f,  -.5f},  {.8f, .1f, .1f}},
                {{.5f,  .5f,  .5f},   {.8f, .1f, .1f}},
                {{-.5f, .5f,  .5f},   {.8f, .1f, .1f}},
                {{-.5f, .5f,  -.5f},  {.8f, .1f, .1f}},
                {{.5f,  .5f,  -.5f},  {.8f, .1f, .1f}},
                {{.5f,  .5f,  .5f},   {.8f, .1f, .1f}},

                // nose face (blue)
                {{-.5f, -.5f, 0.5f},  {.1f, .1f, .8f}},
                {{.5f,  .5f,  0.5f},  {.1f, .1f, .8f}},
                {{-.5f, .5f,  0.5f},  {.1f, .1f, .8f}},
                {{-.5f, -.5f, 0.5f},  {.1f, .1f, .8f}},
                {{.5f,  -.5f, 0.5f},  {.1f, .1f, .8f}},
                {{.5f,  .5f,  0.5f},  {.1f, .1f, .8f}},

                // tail face (green)
                {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                {{.5f,  .5f,  -0.5f}, {.1f, .8f, .1f}},
                {{-.5f, .5f,  -0.5f}, {.1f, .8f, .1f}},
                {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                {{.5f,  -.5f, -0.5f}, {.1f, .8f, .1f}},
                {{.5f,  .5f,  -0.5f}, {.1f, .8f, .1f}},
        };
        auto model = std::make_shared<OveModel>(oveDevice, vertices);

        auto cube = OveGameObject::createGameObject();
        cube.model = model;
//        cube.color = {0.1f, 0.8f, 0.1f};
        cube.transform.translation = {0.0f, 0.0f, 2.5f};
        cube.transform.scale = {0.5f, 0.5f, 0.5f};

        gameObjects.push_back(std::move(cube));
    }

    void FirstApp::run() {
        SimpleRenderSystem simpleRenderSystem{oveDevice, oveRenderer.getSwapChainRenderPass()};
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
                oveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                oveRenderer.endSwapChainRenderPass(commandBuffer);
                oveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(oveDevice.device());
    }
}