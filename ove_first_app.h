#pragma once

#include "ove_window.h"
#include "ove_pipeline.h"
#include "ove_device.h"

namespace ove {
    class FirstApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run();

    private:
        OveWindow oveWindow{WIDTH, HEIGHT, "Hello Vulkan"};
        OveDevice oveDevice{oveWindow};
        OvePipeline ovePipeline{
            oveDevice,
            "../shaders/simple_shader.vert.spv",
            "../shaders/simple_shader.frag.spv",
            OvePipeline::defaultConfigInfo(WIDTH, HEIGHT)
            };
    };
}