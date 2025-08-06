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

void RasterizationShaderProgram::Init(const VkDevice device, const VkPipelineLayout pipelineLayout, RenderPipelineBuilder& pipelineBuilder)
{
    _device = device;

    _pipelineLayout = pipelineLayout;

    pipelineBuilder.SetShaders(_vert, _geom, _frag, _tesc, _tese);

    _pipeline = pipelineBuilder.GetPipeline();
}

void RasterizationShaderProgram::Draw(const VkCommandBuffer cmd, const VkExtent2D extent)
{
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

    //set dynamic viewport and scissor
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = extent.width;
    viewport.height = extent.height;
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

    vkCmdEndRendering(cmd);
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
    if (_vert) data.Merge(Reflect(*_vert));
    if (_frag) data.Merge(Reflect(*_frag));
    if (_geom) data.Merge(Reflect(*_geom));
    if (_tesc) data.Merge(Reflect(*_tesc));
    if (_tese) data.Merge(Reflect(*_tese));
    return data;
}

// Private Fields

// Private Methods

void RasterizationShaderProgram::Cleanup()
{
    if (_vert) _vert.reset();
    if (_geom) _geom.reset();
    if (_frag) _frag.reset();
    if (_tesc) _tesc.reset();
    if (_tese) _tese.reset();

    if (_device)
    {
        if (_pipelineLayout)
            vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
        if (_pipeline)
            vkDestroyPipeline(_device, _pipeline, nullptr);
    }
}

} // namespace velecs::graphics
