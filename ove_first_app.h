#pragma once

#include "ove_window.h"
#include "ove_pipeline.h"

namespace ove {
    class FirstApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run();

    private:
        OveWindow window{WIDTH, HEIGHT, "Hello Vulkan"};
        OvePipeline pipeline{"../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv"};
    };
}