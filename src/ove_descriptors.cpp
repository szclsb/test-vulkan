#include "ove_descriptors.h"

#include <cassert>
#include <stdexcept>

namespace ove {

// *************** Descriptor Set Layout Builder *********************

    OveDescriptorSetLayout::Builder &OveDescriptorSetLayout::Builder::addBinding(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stageFlags,
            uint32_t count) {
        assert(bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<OveDescriptorSetLayout> OveDescriptorSetLayout::Builder::build() const {
        return std::make_unique<OveDescriptorSetLayout>(oveDevice, bindings);
    }

// *************** Descriptor Set Layout *********************

    OveDescriptorSetLayout::OveDescriptorSetLayout(
            OveDevice &oveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
            : oveDevice{oveDevice}, bindings{bindings} {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv: bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
                oveDevice.device(),
                &descriptorSetLayoutInfo,
                nullptr,
                &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    OveDescriptorSetLayout::~OveDescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(oveDevice.device(), descriptorSetLayout, nullptr);
    }

// *************** Descriptor Pool Builder *********************

    OveDescriptorPool::Builder &OveDescriptorPool::Builder::addPoolSize(
            VkDescriptorType descriptorType, uint32_t count) {
        poolSizes.push_back({descriptorType, count});
        return *this;
    }

    OveDescriptorPool::Builder &OveDescriptorPool::Builder::setPoolFlags(
            VkDescriptorPoolCreateFlags flags) {
        poolFlags = flags;
        return *this;
    }

    OveDescriptorPool::Builder &OveDescriptorPool::Builder::setMaxSets(uint32_t count) {
        maxSets = count;
        return *this;
    }

    std::unique_ptr<OveDescriptorPool> OveDescriptorPool::Builder::build() const {
        return std::make_unique<OveDescriptorPool>(oveDevice, maxSets, poolFlags, poolSizes);
    }

// *************** Descriptor Pool *********************

    OveDescriptorPool::OveDescriptorPool(
            OveDevice &oveDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize> &poolSizes)
            : oveDevice{oveDevice} {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(oveDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    OveDescriptorPool::~OveDescriptorPool() {
        vkDestroyDescriptorPool(oveDevice.device(), descriptorPool, nullptr);
    }

    bool OveDescriptorPool::allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(oveDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void OveDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const {
        vkFreeDescriptorSets(
                oveDevice.device(),
                descriptorPool,
                static_cast<uint32_t>(descriptors.size()),
                descriptors.data());
    }

    void OveDescriptorPool::resetPool() {
        vkResetDescriptorPool(oveDevice.device(), descriptorPool, 0);
    }

// *************** Descriptor Writer *********************

    OveDescriptorWriter::OveDescriptorWriter(OveDescriptorSetLayout &setLayout, OveDescriptorPool &pool)
            : setLayout{setLayout}, pool{pool} {}

    OveDescriptorWriter &OveDescriptorWriter::writeBuffer(
            uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto &bindingDescription = setLayout.bindings[binding];

        assert(
                bindingDescription.descriptorCount == 1 &&
                "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    OveDescriptorWriter &OveDescriptorWriter::writeImage(
            uint32_t binding, VkDescriptorImageInfo *imageInfo) {
        assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto &bindingDescription = setLayout.bindings[binding];

        assert(
                bindingDescription.descriptorCount == 1 &&
                "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    bool OveDescriptorWriter::build(VkDescriptorSet &set) {
        bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        overwrite(set);
        return true;
    }

    void OveDescriptorWriter::overwrite(VkDescriptorSet &set) {
        for (auto &write: writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(pool.oveDevice.device(), writes.size(), writes.data(), 0, nullptr);
    }

}  // namespace lve