#pragma once

#include "ove_model.h"

#include <memory>

namespace ove {
    struct Transform2dComponent {
        glm::vec2 translation;
        glm::vec2 scale;
        float rotation;

        // todo use mat3
        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotationMat {{c, s}, {-s, c}};
            glm::mat2 scaleMat {{scale.x, 0.0f}, {0.0f, scale.y}};
            return rotationMat * scaleMat;
        }
    };

    class OveGameObject {
    public:
        using id_t = unsigned int;

        static OveGameObject createGameObject() {
            static id_t currentId = 0;
            return OveGameObject(currentId++);
        }

        OveGameObject(const OveGameObject &) = delete;
        OveGameObject &operator=(const OveGameObject &) = delete;
        OveGameObject(OveGameObject &&) = default;
        OveGameObject &operator=(OveGameObject &&) = default;

        std::shared_ptr<OveModel> model{};
        glm::vec3 color;
        Transform2dComponent transform2d;

    private:
        OveGameObject(id_t objId): id{objId} {}

        id_t id;
    };
}

