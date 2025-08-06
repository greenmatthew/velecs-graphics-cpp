/// @file    ComputePipelineBuilder.cpp
/// @author  Matthew Green
/// @date    2025-07-25 15:39:46
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/ComputePipelineBuilder.hpp"

#include <iostream>
#include <sstream>

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

ComputePipelineBuilder& ComputePipelineBuilder::SetComputeShader(const std::shared_ptr<ComputeShader>& compShader)
{
    _compShader = compShader;
    return *this;
}

// Protected Fields

// Protected Methods

VkPipeline ComputePipelineBuilder::CreatePipeline()
{
    VkComputePipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    info.pNext = nullptr;
    info.layout = _pipelineLayout;
    info.stage = _compShader->GetCreateInfo(_device);

    VkPipeline pipeline;
    VkResult result = vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline);
    if (result != VK_SUCCESS)
    {
        std::ostringstream oss;
        oss << "Failed to create pipeline layout: " << result;
        throw std::runtime_error(oss.str());
    }

    return pipeline;
}

// Private Fields

// Private Methods

} // namespace velecs::graphics
