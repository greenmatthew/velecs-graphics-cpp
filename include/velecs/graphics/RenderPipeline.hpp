/// @file    RenderPipeline.hpp
/// @author  Matthew Green
/// @date    2025-07-18 15:33:33
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Shader/Shader.hpp"

#include <vulkan/vulkan_core.h>

#include <vector>

namespace velecs::graphics {

/// @class RenderPipeline
/// @brief Brief description.
///
/// Rest of description.
class RenderPipeline {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    inline RenderPipeline() { SetDefaultValues(); }

    /// @brief Default deconstructor.
    ~RenderPipeline() = default;

    // Delete copy operations
    RenderPipeline(const RenderPipeline&) = delete;
    RenderPipeline& operator=(const RenderPipeline&) = delete;

    // Allow move operations  
    RenderPipeline(RenderPipeline&& other) noexcept = default;
    RenderPipeline& operator=(RenderPipeline&& other) noexcept = default;

    // Public Methods

    RenderPipeline& SetDevice(const VkDevice device);

    RenderPipeline& SetRenderPass(const VkRenderPass renderPass);

    /// @brief Sets viewport and scissor from window extent (most common case)
    RenderPipeline& SetViewport(const VkExtent2D extent);

    /// @brief Custom viewport (for advanced cases)
    RenderPipeline& SetViewport(float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

    /// @brief Sets scissor rectangle independently of viewport
    RenderPipeline& SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);

    RenderPipeline& SetPipelineLayout(const VkPipelineLayout pipelineLayout);

    /// @brief Sets vertex input description
    RenderPipeline& SetVertexInput(const VkPipelineVertexInputStateCreateInfo& vertexInput);

    /// @brief Adds a shader stage
    RenderPipeline& AddShader(const Shader& shader);

    /// @brief Adds multiple shaders at once
    RenderPipeline& AddShaders(const std::vector<std::reference_wrapper<const Shader>>& shaders);

    /// @brief Sets primitive topology (triangles, lines, etc.)
    RenderPipeline& SetTopology(VkPrimitiveTopology topology);

    /// @brief Sets polygon mode (fill, wireframe, points)
    RenderPipeline& SetPolygonMode(VkPolygonMode mode);

    /// @brief Sets face culling
    RenderPipeline& SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE);

    /// @brief Enables/disables depth testing
    RenderPipeline& SetDepthTest(bool enable, bool write = true, VkCompareOp compareOp = VK_COMPARE_OP_LESS);

    /// @brief Enables simple alpha blending (for UI/transparency)
    RenderPipeline& EnableAlphaBlending();

    /// @brief Disables blending (opaque rendering)
    RenderPipeline& DisableBlending();

    /// @brief Creates the actual Vulkan pipeline
    VkPipeline GetPipeline();

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    VkDevice _device{VK_NULL_HANDLE};
    VkRenderPass _renderPass{VK_NULL_HANDLE};
    VkPipelineLayout _pipelineLayout{VK_NULL_HANDLE}; /// @brief The layout of the pipeline, describing shader stages and more.

    std::vector<VkPipelineShaderStageCreateInfo> _shaderStages; /// @brief Collection of shader stages used in the pipeline.
    VkPipelineVertexInputStateCreateInfo _vertexInputInfo; /// @brief Description of the format of the vertex data.
    VkPipelineInputAssemblyStateCreateInfo _inputAssembly; /// @brief Information about the type of geometry primitives to be processed.
    VkViewport _viewport; /// @brief The viewport transformation to be applied.
    VkRect2D _scissor; /// @brief The scissor test to be applied.
    VkPipelineRasterizationStateCreateInfo _rasterizer; /// @brief Rasterization state parameters.
    VkPipelineColorBlendAttachmentState _colorBlendAttachment; /// @brief Color blending settings for the pipeline.
    VkPipelineMultisampleStateCreateInfo _multisampling; /// @brief Multisampling state parameters.
    VkPipelineDepthStencilStateCreateInfo _depthStencil;

    // Private Methods

    void SetDefaultValues();

    /// @brief Validates that all required state is set
    void ValidateState();
};

} // namespace velecs::graphics
