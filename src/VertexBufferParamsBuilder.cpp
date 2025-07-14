/// @file    VertexBufferParamsBuilder.cpp
/// @author  Matthew Green
/// @date    2025-07-12 16:40:51
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/VertexBufferParamsBuilder.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

VertexBufferParamsBuilder& VertexBufferParamsBuilder::AddBinding(
    uint32_t stride, 
    VkVertexInputRate inputRate, 
    BindingConfigurator configurator)
{
    uint32_t bindingIndex = static_cast<uint32_t>(_bindings.size());
    
    // Create the binding using private constructor
    VertexInputBinding binding(*this, bindingIndex, stride, inputRate);
    
    // Add to bindings vector
    _bindings.push_back(binding._bindingDescription);
    
    // Configure attributes
    configurator(binding);
    
    return *this;
}

VkPipelineVertexInputStateCreateInfo VertexBufferParamsBuilder::GetCreateInfo()
{
    VkPipelineVertexInputStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    info.pNext = nullptr;
    // "`flags` is reserved for future use." according to the Vulkan spec:
    // https://registry.khronos.org/vulkan/specs/latest/man/html/VkPipelineVertexInputStateCreateInfo.html
    info.flags = 0;
    
    info.pVertexBindingDescriptions = _bindings.data();
    info.vertexBindingDescriptionCount = static_cast<uint32_t>(_bindings.size());
    
    info.pVertexAttributeDescriptions = _attributes.data();
    info.vertexAttributeDescriptionCount = static_cast<uint32_t>(_attributes.size());
    
    return info;
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::graphics
