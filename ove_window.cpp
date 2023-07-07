#include "ove_window.h"

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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }
}