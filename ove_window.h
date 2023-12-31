#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <string>

namespace ove {
    class OveWindow {
    public:
        OveWindow(int width, int height, std::string name);
        ~OveWindow();

        OveWindow(const OveWindow &) = delete;
        OveWindow &operator=(const OveWindow &) = delete;

        bool shouldClose() {
            return glfwWindowShouldClose(window);
        }

        VkExtent2D getExtent() {
            return { static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
        bool wasResized() {return resized;}
        void resetResizeFlag() {resized = false;}

    private:
        static void resizeCallback(GLFWwindow *window, int width, int height);
        void initWindow();

        int width;
        int height;
        bool resized = false;

        std::string windowName;
        GLFWwindow *window;
    };
}