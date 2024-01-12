#pragma once

#include "ove_device.h"
#include "ove_buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace ove {

    class OveModel {
    public:
        struct Vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 uv;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex &other) const {
                return position == other.position && normal == other.normal && uv == other.uv && color == other.color;
            }
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices {};

            void loadModel(const std::string &fileName);
        };

        OveModel(OveDevice &device, const OveModel::Builder &builder);
        ~OveModel();

        static std::unique_ptr<OveModel> createModelFromFile(OveDevice &device, const std::string &fileName);

        OveModel(const OveModel &) = delete;
        OveModel &operator=(const OveModel &) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);

        OveDevice &oveDevice;

        std::unique_ptr<OveBuffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<OveBuffer> indexBuffer;
        uint32_t indexCount;
    };
}
