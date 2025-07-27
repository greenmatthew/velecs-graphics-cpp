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
    RenderPipelineBuilder() = default;

    /// @brief Default deconstructor.
    ~RenderPipelineBuilder() = default;

    // Delete copy operations
    RenderPipelineBuilder(const RenderPipelineBuilder&) = delete;
    RenderPipelineBuilder& operator=(const RenderPipelineBuilder&) = delete;

    // Allow move operations  
    RenderPipelineBuilder(RenderPipelineBuilder&& other) noexcept = default;
    RenderPipelineBuilder& operator=(RenderPipelineBuilder&& other) noexcept = default;

    // Public Methods

    /// @brief Adds a shader stage to the pipeline.
    /// @param shader Shared pointer to a compiled and valid shader
    /// @return Reference to this builder for method chaining
    /// @details The shader must be compiled and valid before adding. Invalid shaders
    ///          will be caught during validation. Duplicate shader stages may be
    ///          rejected depending on the pipeline type.
    RenderPipelineBuilder& AddShader(const std::shared_ptr<VertexShader>& shader);

    /// @brief Adds a shader stage to the pipeline.
    /// @param shader Shared pointer to a compiled and valid shader
    /// @return Reference to this builder for method chaining
    /// @details The shader must be compiled and valid before adding. Invalid shaders
    ///          will be caught during validation. Duplicate shader stages may be
    ///          rejected depending on the pipeline type.
    RenderPipelineBuilder& AddShader(const std::shared_ptr<GeometryShader>& shader);

    /// @brief Adds a shader stage to the pipeline.
    /// @param shader Shared pointer to a compiled and valid shader
    /// @return Reference to this builder for method chaining
    /// @details The shader must be compiled and valid before adding. Invalid shaders
    ///          will be caught during validation. Duplicate shader stages may be
    ///          rejected depending on the pipeline type.
    RenderPipelineBuilder& AddShader(const std::shared_ptr<FragmentShader>& shader);

    /// @brief Adds a shader stage to the pipeline.
    /// @param shader Shared pointer to a compiled and valid shader
    /// @return Reference to this builder for method chaining
    /// @details The shader must be compiled and valid before adding. Invalid shaders
    ///          will be caught during validation. Duplicate shader stages may be
    ///          rejected depending on the pipeline type.
    RenderPipelineBuilder& AddShader(const std::shared_ptr<TessellationControlShader>& shader);

    /// @brief Adds a shader stage to the pipeline.
    /// @param shader Shared pointer to a compiled and valid shader
    /// @return Reference to this builder for method chaining
    /// @details The shader must be compiled and valid before adding. Invalid shaders
    ///          will be caught during validation. Duplicate shader stages may be
    ///          rejected depending on the pipeline type.
    RenderPipelineBuilder& AddShader(const std::shared_ptr<TessellationEvaluationShader>& shader);

    RenderPipelineBuilder& SetRenderPass(const VkRenderPass renderPass);

    /// @brief Sets viewport and scissor from window extent (most common case)
    RenderPipelineBuilder& SetViewport(const VkExtent2D extent);

    /// @brief Custom viewport (for advanced cases)
    RenderPipelineBuilder& SetViewport(float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

    /// @brief Sets scissor rectangle independently of viewport
    RenderPipelineBuilder& SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);

    /// @brief Sets vertex input description
    RenderPipelineBuilder& SetVertexInput(const VkPipelineVertexInputStateCreateInfo& vertexInput);

    /// @brief Sets primitive topology (triangles, lines, etc.)
    RenderPipelineBuilder& SetTopology(VkPrimitiveTopology topology);

    /// @brief Sets polygon mode (fill, wireframe, points)
    RenderPipelineBuilder& SetPolygonMode(VkPolygonMode mode);

    /// @brief Sets face culling
    RenderPipelineBuilder& SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE);

    /// @brief Enables/disables depth testing
    RenderPipelineBuilder& SetDepthTest(bool enable, bool write = true, VkCompareOp compareOp = VK_COMPARE_OP_LESS);

    /// @brief Enables simple alpha blending (for UI/transparency)
    RenderPipelineBuilder& EnableAlphaBlending();

    /// @brief Disables blending (opaque rendering)
    RenderPipelineBuilder& DisableBlending();

protected:
    // Protected Fields

    std::vector<std::shared_ptr<Shader>> _shaders;    /// @brief Collection of shaders to be used in the pipeline

    // Protected Methods

    /// @brief Validates that all required state is set
    virtual void ValidateState() override;

    /// @brief 
    /// @return
    /// /// Note Assumes a valid state and described by `ValidateState()`. 
    virtual VkPipeline CreatePipeline() override;

    /// @brief Converts stored shaders to Vulkan pipeline shader stage create info structures.
    /// @return Vector of VkPipelineShaderStageCreateInfo for all shaders
    /// @details Iterates through all added shaders and returns
    ///          the create info structures needed for Vulkan pipeline creation.
    std::vector<VkPipelineShaderStageCreateInfo> GetShaderStageCreateInfos();

private:
    // Private Fields

    VkRenderPass _renderPass{VK_NULL_HANDLE};

    VkPipelineVertexInputStateCreateInfo _vertexInputInfo; /// @brief Description of the format of the vertex data.
    VkPipelineInputAssemblyStateCreateInfo _inputAssembly; /// @brief Information about the type of geometry primitives to be processed.
    VkViewport _viewport; /// @brief The viewport transformation to be applied.
    VkRect2D _scissor; /// @brief The scissor test to be applied.
    VkPipelineRasterizationStateCreateInfo _rasterizer; /// @brief Rasterization state parameters.
    VkPipelineColorBlendAttachmentState _colorBlendAttachment; /// @brief Color blending settings for the pipeline.
    VkPipelineMultisampleStateCreateInfo _multisampling; /// @brief Multisampling state parameters.
    VkPipelineDepthStencilStateCreateInfo _depthStencil;

    // Private Methods
};

} // namespace velecs::graphics
