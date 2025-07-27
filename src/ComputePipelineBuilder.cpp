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
    if (!compShader->IsValid())
    {
        throw std::runtime_error("Shader is not valid");
    }

    VkShaderStageFlagBits stage = compShader->GetStage();
    
    if (stage != VK_SHADER_STAGE_COMPUTE_BIT)
    {
        throw std::runtime_error("Shader must be a compute shader for compute pipeline");
    }

    _compShader = compShader;
    return *this;
}

// Protected Fields

// Protected Methods

void ComputePipelineBuilder::ValidateState()
{
    if (_compShader != nullptr)
        throw std::runtime_error("No compute shader set");
}

VkPipeline ComputePipelineBuilder::CreatePipeline()
{
    VkComputePipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    info.pNext = nullptr;
    info.layout = _pipelineLayout;
    info.stage = _compShader->GetCreateInfo();

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
