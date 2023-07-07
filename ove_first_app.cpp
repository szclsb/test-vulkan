#include "ove_first_app.h"

namespace ove {
    void FirstApp::run() {
        while(!window.shouldClose()) {
            glfwPollEvents();
        }
    }
}