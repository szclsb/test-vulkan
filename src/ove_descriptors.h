#pragma once

#include "ove_device.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace ove {
    class OveDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(OveDevice &oveDevice) : oveDevice{oveDevice} {}

            Builder &addBinding(
                    uint32_t binding,
                    VkDescriptorType descriptorType,
                    VkShaderStageFlags stageFlags,
                    uint32_t count = 1);

            std::unique_ptr<OveDescriptorSetLayout> build() const;

        private:
            OveDevice &oveDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        OveDescriptorSetLayout(OveDevice &oveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~OveDescriptorSetLayout();
        OveDescriptorSetLayout(const OveDescriptorSetLayout &) = delete;
        OveDescriptorSetLayout &operator=(const OveDescriptorSetLayout &) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        OveDevice &oveDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class OveDescriptorWriter;
    };

    class OveDescriptorPool {
    public:
        class Builder {
        public:
            Builder(OveDevice &oveDevice) : oveDevice{oveDevice} {}

            Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &setMaxSets(uint32_t count);
            std::unique_ptr<OveDescriptorPool> build() const;

        private:
            OveDevice &oveDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        OveDescriptorPool(
                OveDevice &oveDevice,
                uint32_t maxSets,
                VkDescriptorPoolCreateFlags poolFlags,
                const std::vector<VkDescriptorPoolSize> &poolSizes);
        ~OveDescriptorPool();

        OveDescriptorPool(const OveDescriptorPool &) = delete;
        OveDescriptorPool &operator=(const OveDescriptorPool &) = delete;

        bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;
        void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;
        void resetPool();

    private:
        OveDevice &oveDevice;
        VkDescriptorPool descriptorPool;

        friend class OveDescriptorWriter;
    };

    class OveDescriptorWriter {
    public:
        OveDescriptorWriter(OveDescriptorSetLayout &setLayout, OveDescriptorPool &pool);

        OveDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        OveDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);
        bool build(VkDescriptorSet &set);
        void overwrite(VkDescriptorSet &set);

    private:
        OveDescriptorSetLayout &setLayout;
        OveDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };
}
