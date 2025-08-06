/// @file    ComputeShaderProgram.cpp
/// @author  Matthew Green
/// @date    2025-07-25 14:58:43
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/ShaderPrograms/ComputeShaderProgram.hpp"

#include "velecs/graphics/Shader/Reflection/ShaderReflector.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

bool ComputeShaderProgram::IsComplete() const
{
    return _comp ? true : false;
}

size_t ComputeShaderProgram::GetStageCount() const
{
    return _comp ? 1 : 0;
}

void ComputeShaderProgram::SetComputeShader(const std::shared_ptr<ComputeShader>& shader)
{
    _comp = shader;
}

void ComputeShaderProgram::SetDescriptor(const VkDescriptorSetLayout descriptorSetLayout, const VkDescriptorSet descriptorSet)
{
    _descriptorSetLayout = descriptorSetLayout;
    _descriptorSet = descriptorSet;
}

void ComputeShaderProgram::SetGroupCount(const uint32_t x, const uint32_t y/* = 1*/, const uint32_t z/* = 1*/)
{
    _numGroupsX = x;
    _numGroupsY = y;
    _numGroupsZ = z;
}

void ComputeShaderProgram::Init(const VkDevice device)
{
    assert(device != VK_NULL_HANDLE && "Device needs to be valid");

    _device = device;
    InitPipelineLayout();
    InitPipeline();
    _initialized = true;
}

void ComputeShaderProgram::Dispatch(const VkCommandBuffer cmd)
{
    assert(cmd && "Command buffer must be valid");
    assert(_initialized && "Failed to call Init()");
    assert(_numGroupsX.has_value() && "Number of groups on X must be valid");
    assert(_numGroupsY.has_value() && "Number of groups on Y must be valid");
    assert(_numGroupsZ.has_value() && "Number of groups on Z must be valid");

    // Bind the gradient drawing compute pipeline
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _pipeline);

    // Bind the descriptor set containing the draw image for the compute pipeline
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _pipelineLayout, 0, 1, &_descriptorSet, 0, nullptr);
    
    // Only push constants if they are set
    if (_pushConstant.has_value())
    {
        vkCmdPushConstants(
            cmd,
            _pipelineLayout,
            VK_SHADER_STAGE_COMPUTE_BIT,
            0,
            _pushConstant->GetSize(),
            _pushConstant->GetRawData()
        );
    }

    vkCmdDispatch(cmd, _numGroupsX.value(), _numGroupsY.value(), _numGroupsZ.value());
}

// Protected Fields

// Protected Methods

bool ComputeShaderProgram::ValidateShaders() const
{
    return _comp && _comp->IsValid();
}

VkShaderStageFlags ComputeShaderProgram::GetShaderStages()
{
    return VK_SHADER_STAGE_COMPUTE_BIT;
}

ShaderReflectionData ComputeShaderProgram::GetReflectionData()
{
    return Reflect(*_comp.get());
}

void ComputeShaderProgram::InitPipelineLayout()
{
    VkPipelineLayoutCreateInfo computeLayout{};
    computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    computeLayout.pNext = nullptr;
    computeLayout.pSetLayouts = &_descriptorSetLayout;
    computeLayout.setLayoutCount = 1;
    
    if (_pushConstant.has_value())
    {
        computeLayout.pPushConstantRanges = &_pushConstant->GetRange();
        computeLayout.pushConstantRangeCount = 1;
    }

    VkResult result = vkCreatePipelineLayout(_device, &computeLayout, nullptr, &_pipelineLayout);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout: " + std::to_string(result));
    }
}

void ComputeShaderProgram::InitPipeline()
{
    _pipeline = ComputePipelineBuilder{}
        .SetDevice(_device)
        .SetPipelineLayout(_pipelineLayout)
        .SetComputeShader(_comp)
        .GetPipeline();
}

// Private Fields

// Private Methods

} // namespace velecs::graphics
