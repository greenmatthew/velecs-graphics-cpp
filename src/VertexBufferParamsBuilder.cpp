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

const VkPipelineVertexInputStateCreateInfo& VertexBufferParamsBuilder::GetCreateInfo() const
{
    if (!_createInfoValid) {
        _createInfo = {};
        _createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        _createInfo.pNext = nullptr;
        _createInfo.flags = 0;
        
        _createInfo.pVertexBindingDescriptions = _bindings.data();
        _createInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(_bindings.size());
        
        _createInfo.pVertexAttributeDescriptions = _attributes.data();
        _createInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(_attributes.size());
        
        _createInfoValid = true;
    }
    
    return _createInfo;
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::graphics
