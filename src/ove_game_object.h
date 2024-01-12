#pragma once

#include "ove_model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace ove {
    struct TransformComponent {
        glm::vec3 translation{0.0f, 0.0f, 0.0f};
        glm::vec3 rotation{0.0f, 0.0f, 0.0f};
        float scale{1.0f};  //simpler light calculation in shaders

        // Matrix correspond to translate * Ry * Rx * Rz * scale
        // Rotation convention uses tait-bryan angles with axis order Y(1) X(2) Z(3)
        glm::mat4 evaluate() {
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);
            return glm::mat4{
                    {
                            scale * (c1 * c3 + s1 * s2 * s3),
                            scale * (c2 * s3),
                            scale * (c1 * s2 * s3 - c3 * s1),
                            0.0f
                    },
                    {
                            scale * (c3 * s1 * s2 - c1 * s3),
                            scale * (c2 * c3),
                            scale * (c1 * c3 * s2 + s1 * s3),
                            0.0f
                    },
                    {
                            scale * (c2 * s1),
                            scale * (-s2),
                            scale * (c1 * c2),
                            0.0f
                    },
                    {translation.x, translation.y, translation.z, 1.0f}
            };
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
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        OveGameObject(id_t objId) : id{objId} {}

        id_t id;
    };
}

