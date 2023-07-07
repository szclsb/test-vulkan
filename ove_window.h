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

    private:
        void initWindow();

        const int width;
        const int height;

        std::string windowName;
        GLFWwindow *window;
    };
}