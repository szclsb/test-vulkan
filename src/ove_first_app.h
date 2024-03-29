#pragma once

#include "ove_descriptors.h"
#include "ove_device.h"
#include "ove_game_object.h"
#include "ove_renderer.h"
#include "ove_window.h"
#include "ove_scene.h"

#include <memory>
#include <vector>

namespace ove {
    class FirstApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp &) = delete;
        FirstApp &operator=(const FirstApp &) = delete;

        void run();

    private:
        void loadGameObjects();
        void loadLights();

        OveWindow oveWindow{WIDTH, HEIGHT, "Hello Vulkan"};
        OveDevice oveDevice{oveWindow};
        OveRenderer oveRenderer{oveWindow, oveDevice};

        // note: order of declaration matters
        std::unique_ptr<OveDescriptorPool> globalPool;
        std::shared_ptr<Scene> scene;
    };
}