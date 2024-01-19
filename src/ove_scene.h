#pragma once

#include "ove_camera.h"
#include "ove_game_object.h"

#include <vector>

namespace ove {
    struct Scene {
        OveCamera camera;
        std::vector<OveGameObject> gameObjects;
        std::vector<OveGameObject> lights;
    };
}