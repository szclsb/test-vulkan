#include "ove_window.h"

#include <stdexcept>

namespace ove {
    OveWindow::OveWindow(int width, int height, std::string name) : width{width}, height{height}, windowName{name} {
        initWindow();
    }

    OveWindow::~OveWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void OveWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // Disable OpenGL
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, resizeCallback);
    }

    void OveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface");
        }
    }

    void OveWindow::resizeCallback(GLFWwindow *window, int width, int height) {
        auto oveWindow = reinterpret_cast<OveWindow *>(glfwGetWindowUserPointer(window));
        oveWindow->resized = true;
        oveWindow->width = width;
        oveWindow->height = height;
    }
}