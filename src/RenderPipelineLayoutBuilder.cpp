/// @file    RenderPipelineLayoutBuilder.cpp
/// @author  Matthew Green
/// @date    2025-07-20 11:17:11
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/RenderPipelineLayoutBuilder.hpp"

#include <stdexcept>
#include <string>

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

RenderPipelineLayoutBuilder& RenderPipelineLayoutBuilder::AddDescriptorSetLayout(VkDescriptorSetLayout setLayout)
{
    _descriptorSetLayouts.push_back(setLayout);
    return *this;
}

RenderPipelineLayoutBuilder& RenderPipelineLayoutBuilder::AddDescriptorSetLayouts(const std::vector<VkDescriptorSetLayout>& setLayouts)
{
    _descriptorSetLayouts.insert(_descriptorSetLayouts.end(), setLayouts.begin(), setLayouts.end());
    return *this;
}

VkPipelineLayout RenderPipelineLayoutBuilder::GetLayout()
{
    if (_device == VK_NULL_HANDLE) throw std::runtime_error("Invalid device handle");

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.pNext = VK_NULL_HANDLE;
    layoutInfo.flags = 0;

    // Set up descriptor set layouts
    layoutInfo.setLayoutCount = static_cast<uint32_t>(_descriptorSetLayouts.size());
    layoutInfo.pSetLayouts = _descriptorSetLayouts.empty() ? nullptr : _descriptorSetLayouts.data();

    // Set up push constant ranges
    layoutInfo.pushConstantRangeCount = static_cast<uint32_t>(_pushConstantRanges.size());
    layoutInfo.pPushConstantRanges = _pushConstantRanges.empty() ? nullptr : _pushConstantRanges.data();

    VkPipelineLayout layout{VK_NULL_HANDLE};
    VkResult result = vkCreatePipelineLayout(_device, &layoutInfo, nullptr, &layout);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout: " + std::to_string(result));
    }

    return layout;
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::graphics
