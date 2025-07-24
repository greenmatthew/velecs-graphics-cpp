/// @file    DescriptorLayoutBuilder.cpp
/// @author  Matthew Green
/// @date    2025-07-24 14:51:26
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/DescriptorLayoutBuilder.hpp"

#include <iostream>

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

DescriptorLayoutBuilder& DescriptorLayoutBuilder::AddBinding(const uint32_t binding, const VkDescriptorType type)
{
    VkDescriptorSetLayoutBinding newBinding{};
    newBinding.binding = 0;
    newBinding.descriptorCount = 1;
    newBinding.descriptorType = type;

    _bindings.push_back(newBinding);

    return *this;
}

DescriptorLayoutBuilder& DescriptorLayoutBuilder::Clear()
{
    _bindings.clear();
    
    return *this;
}

VkDescriptorSetLayout DescriptorLayoutBuilder::Build(
    const VkDevice device,
    const VkShaderStageFlags stages,
    const void* const pNext/* = nullptr*/,
    const VkDescriptorSetLayoutCreateFlags flags/* = 0*/
)
{
    for (auto& binding : _bindings)
    {
        binding.stageFlags = stages;
    }

    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pNext = pNext;
    info.pBindings = _bindings.data();
    info.bindingCount = static_cast<uint32_t>(_bindings.size());
    info.flags = flags;

    VkDescriptorSetLayout setLayout{};
    VkResult result = vkCreateDescriptorSetLayout(device, &info, nullptr, &setLayout);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create descriptor set layout: " << result << std::endl;
    }

    return setLayout;
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::graphics
