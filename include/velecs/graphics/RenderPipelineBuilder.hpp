/// @file    RenderPipelineBuilder.hpp
/// @author  Matthew Green
/// @date    2025-07-18 15:33:33
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/PipelineBuilderBase.hpp"

#include "velecs/graphics/Shader/Shaders/VertexShader.hpp"
#include "velecs/graphics/Shader/Shaders/GeometryShader.hpp"
#include "velecs/graphics/Shader/Shaders/FragmentShader.hpp"
#include "velecs/graphics/Shader/Shaders/TessellationControlShader.hpp"
#include "velecs/graphics/Shader/Shaders/TessellationEvaluationShader.hpp"

#include <vulkan/vulkan_core.h>

#include <vector>
#include <optional>

namespace velecs::graphics {

/// @class RenderPipelineBuilder
/// @brief Brief description.
///
/// Rest of description.
class RenderPipelineBuilder : public PipelineBuilderBase<RenderPipelineBuilder> {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    RenderPipelineBuilder() { Clear(); }

    /// @brief Default deconstructor.
    ~RenderPipelineBuilder() = default;

    // Delete copy operations
    RenderPipelineBuilder(const RenderPipelineBuilder&) = delete;
    RenderPipelineBuilder& operator=(const RenderPipelineBuilder&) = delete;

    // Allow move operations  
    RenderPipelineBuilder(RenderPipelineBuilder&& other) noexcept = default;
    RenderPipelineBuilder& operator=(RenderPipelineBuilder&& other) noexcept = default;

    // Public Methods

    RenderPipelineBuilder& SetShaders(const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages);

    /// @brief Sets vertex input description
    RenderPipelineBuilder& SetVertexInput(const VkPipelineVertexInputStateCreateInfo& vertexInput);

    /// @brief Sets primitive topology (triangles, lines, etc.)
    RenderPipelineBuilder& SetTopology(VkPrimitiveTopology topology);

    /// @brief Sets polygon mode (fill, wireframe, points)
    RenderPipelineBuilder& SetPolygonMode(VkPolygonMode mode);

    /// @brief Sets face culling
    RenderPipelineBuilder& SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE);

    RenderPipelineBuilder& SetMultisamplingNone();

    /// @brief Disables blending (opaque rendering)
    RenderPipelineBuilder& DisableBlending();

    /// @brief Enables simple alpha blending (for UI/transparency)
    RenderPipelineBuilder& EnableAlphaBlending();

    RenderPipelineBuilder& SetColorAttachmentFormat(const VkFormat format);

    RenderPipelineBuilder& SetDepthFormat(const VkFormat format);

    RenderPipelineBuilder& DisableDepthTest();

    /// @brief Enables/disables depth testing
    RenderPipelineBuilder& SetDepthTest(bool enable, bool write = true, VkCompareOp compareOp = VK_COMPARE_OP_LESS);

    void Clear();

protected:
    // Protected Fields

    // Protected Methods

    /// @brief 
    /// @return
    /// /// Note Assumes a valid state and described by `ValidateState()`. 
    virtual VkPipeline CreatePipeline() override;

private:
    // Private Fields

    std::vector<VkPipelineShaderStageCreateInfo> _shaderStages; /// @brief Collection of shader stages to be used in the pipeline

    VkPipelineVertexInputStateCreateInfo _vertexInputInfo; /// @brief Description of the format of the vertex data.
    
    VkPipelineInputAssemblyStateCreateInfo _inputAssembly; /// @brief Information about the type of geometry primitives to be processed.

    VkPipelineRasterizationStateCreateInfo _rasterizer; /// @brief Rasterization state parameters.

    VkPipelineMultisampleStateCreateInfo _multisampling; /// @brief Multisampling state parameters.

    VkPipelineDepthStencilStateCreateInfo _depthStencil;

    VkPipelineColorBlendAttachmentState _colorBlendAttachment; /// @brief Color blending settings for the pipeline.

    VkPipelineRenderingCreateInfo _renderInfo;

    VkFormat _colorAttachmentFormat;

    // Private Methods
};

} // namespace velecs::graphics
