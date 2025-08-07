/// @file    RasterizationShaderProgram.cpp
/// @author  Matthew Green
/// @date    2025-07-14 13:46:15
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/ShaderPrograms/RasterizationShaderProgram.hpp"

#include "velecs/graphics/Shader/Reflection/ShaderReflector.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

bool RasterizationShaderProgram::IsComplete() const
{
    // Must have vertex and fragment shaders
    if (!_vert || !_frag) return false;
    
    // If tessellation is used, must have both control and evaluation shaders
    if ((_tesc != nullptr) != (_tese != nullptr)) return false;
    
    return true;
}

size_t RasterizationShaderProgram::GetStageCount() const
{
    size_t count = 0;
    if (_vert) ++count;
    if (_frag) ++count;
    if (_geom) ++count;
    if (_tesc) ++count;
    if (_tese) ++count;
    return count;
}

void RasterizationShaderProgram::SetVertexShader(const std::shared_ptr<VertexShader>& vert)
{
    _vert = vert;
}

void RasterizationShaderProgram::SetGeometryShader(const std::shared_ptr<GeometryShader>& geom)
{
    _geom = geom;
}

void RasterizationShaderProgram::SetFragmentShader(const std::shared_ptr<FragmentShader>& frag)
{
    _frag = frag;
}

void RasterizationShaderProgram::SetTessellationControlShader(const std::shared_ptr<TessellationControlShader>& tesc)
{
    _tesc = tesc;
}

void RasterizationShaderProgram::SetTessellationEvaluationShader(const std::shared_ptr<TessellationEvaluationShader>& tese)
{
    _tese = tese;
}

void RasterizationShaderProgram::Init(const VkDevice device, const VkFormat colorAttachmentFormat)
{
    if (_initialized) throw std::runtime_error("Cannot call Init() more than once");
    if (device == VK_NULL_HANDLE) throw std::runtime_error("Invalid device handle");
    if (!IsComplete()) throw std::runtime_error("Either no shaders were assigned or there is an invalid combination of shaders");

    _device = device;

    InitPipelineLayout();

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    if (_vert) shaderStages.push_back(_vert->GetCreateInfo(_device));
    if (_frag) shaderStages.push_back(_frag->GetCreateInfo(_device));
    if (_geom) shaderStages.push_back(_geom->GetCreateInfo(_device));
    if (_tesc) shaderStages.push_back(_tesc->GetCreateInfo(_device));
    if (_tese) shaderStages.push_back(_tese->GetCreateInfo(_device));

    pipelineBuilder.SetDevice(_device)
        .SetPipelineLayout(_pipelineLayout)
        .SetShaders(shaderStages)
        .SetColorAttachmentFormat(colorAttachmentFormat)
        ;

    InitPipeline();

    _initialized = true;
}

void RasterizationShaderProgram::Draw(const VkCommandBuffer cmd, const VkExtent2D extent)
{
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

    //set dynamic viewport and scissor
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = extent.width;
    scissor.extent.height = extent.height;

    vkCmdSetScissor(cmd, 0, 1, &scissor);

    //launch a draw command to draw 3 vertices
    vkCmdDraw(cmd, 3, 1, 0, 0);
}

// Protected Fields

// Protected Methods

bool RasterizationShaderProgram::ValidateShaders() const
{
    if (_vert && !_vert->IsValid()) return false;
    if (_frag && !_frag->IsValid()) return false;
    if (_geom && !_geom->IsValid()) return false;
    if (_tesc && !_tesc->IsValid()) return false;
    if (_tese && !_tese->IsValid()) return false;
    
    return true;
}

VkShaderStageFlags RasterizationShaderProgram::GetShaderStages()
{
    VkShaderStageFlags stages = 0;
    if (_vert) stages |= VK_SHADER_STAGE_VERTEX_BIT;
    if (_frag) stages |= VK_SHADER_STAGE_FRAGMENT_BIT;
    if (_geom) stages |= VK_SHADER_STAGE_GEOMETRY_BIT;
    if (_tesc) stages |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    if (_tese) stages |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    return stages;
}

ShaderReflectionData RasterizationShaderProgram::GetReflectionData()
{
    ShaderReflectionData data{};
    if (_vert) data = data.Merge(Reflect(*_vert));
    if (_frag) data = data.Merge(Reflect(*_frag));
    if (_geom) data = data.Merge(Reflect(*_geom));
    if (_tesc) data = data.Merge(Reflect(*_tesc));
    if (_tese) data = data.Merge(Reflect(*_tese));
    return data;
}

// Private Fields

// Private Methods

void RasterizationShaderProgram::InitPipelineLayout()
{
    VkPipelineLayoutCreateInfo graphicsLayout{};
    graphicsLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    graphicsLayout.pNext = VK_NULL_HANDLE;
    graphicsLayout.flags = 0;

    if (_pushConstant.has_value())
    {
        graphicsLayout.pPushConstantRanges = &_pushConstant->GetRange();
        graphicsLayout.pushConstantRangeCount = 1;
    }

    VkResult result = vkCreatePipelineLayout(_device, &graphicsLayout, nullptr, &_pipelineLayout);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout: " + std::to_string(result));
    }
}

void RasterizationShaderProgram::InitPipeline()
{
    _pipeline = pipelineBuilder.GetPipeline();
}

void RasterizationShaderProgram::Cleanup()
{
    if (_vert) _vert.reset();
    if (_geom) _geom.reset();
    if (_frag) _frag.reset();
    if (_tesc) _tesc.reset();
    if (_tese) _tese.reset();

    if (_device != VK_NULL_HANDLE)
    {
        if (_pipelineLayout != VK_NULL_HANDLE)
            vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
        if (_pipeline != VK_NULL_HANDLE)
            vkDestroyPipeline(_device, _pipeline, nullptr);
    }
}

} // namespace velecs::graphics
