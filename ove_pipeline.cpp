#include "ove_pipeline.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace ove {
    OvePipeline::OvePipeline(const std::string &vertFilepath, const std::string &fragFilepath) {
        createGraphicsPipeline(vertFilepath, fragFilepath);
    }

    std::vector<char> OvePipeline::readFile(const std::string &filepath) {
        std::ifstream file{filepath, std::ios::ate | std::ios::binary};

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + filepath);
        }

        auto fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        return buffer;
    }

    void OvePipeline::createGraphicsPipeline(const std::string &vertFilepath, const std::string &fragFilepath) {
        auto vertCode = readFile(vertFilepath);
        auto fragCode = readFile(fragFilepath);

        std::cout << "vertex shader code size: " << vertCode.size() << std::endl;
        std::cout << "fragment shader code size: " << fragCode.size() << std::endl;
    }
}