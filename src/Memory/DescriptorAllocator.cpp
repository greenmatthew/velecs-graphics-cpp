/// @file    DescriptorAllocator.cpp
/// @author  Matthew Green
/// @date    2025-07-24 15:19:04
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Memory/DescriptorAllocator.hpp"

#include <iostream>

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

void DescriptorAllocator::InitPool(const VkDevice device, const uint32_t maxSets, const std::vector<PoolSizeRatio>& poolRatios)
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (PoolSizeRatio ratio : poolRatios)
    {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = ratio.type;
        poolSize.descriptorCount = static_cast<uint32_t>(ratio.ratio * maxSets);
        poolSizes.push_back(poolSize);
    }

    VkDescriptorPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.flags = 0;
    info.maxSets = maxSets;
    info.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    info.pPoolSizes = poolSizes.data();

    VkResult result = vkCreateDescriptorPool(device, &info, nullptr, &_pool);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create descriptor pool: " << result << std::endl;
    }
}

void DescriptorAllocator::ClearDescriptors(const VkDevice device)
{
    VkResult result = vkResetDescriptorPool(device, _pool, 0);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to reset descriptor pool: " << result << std::endl;
    }
}

void DescriptorAllocator::DestroyPool(const VkDevice device)
{
    vkDestroyDescriptorPool(device, _pool, nullptr);
}

VkDescriptorSet DescriptorAllocator::Allocate(const VkDevice device, const VkDescriptorSetLayout layout)
{
    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.descriptorPool = _pool;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &layout;

    VkDescriptorSet set{};
    VkResult result = vkAllocateDescriptorSets(device, &info, &set);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to allocate descriptor sets: " << result << std::endl;
    }

    return set;
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::graphics
