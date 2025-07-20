/// @file    RenderPipeline.cpp
/// @author  Matthew Green
/// @date    2025-07-18 15:43:18
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/RenderPipeline.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

RenderPipeline& RenderPipeline::SetDevice(const VkDevice device)
{
    _device = device;
    return *this;
}

RenderPipeline& RenderPipeline::SetRenderPass(const VkRenderPass renderPass)
{
    _renderPass = renderPass;
    return *this;
}

RenderPipeline& RenderPipeline::SetViewport(VkExtent2D extent)
{
    _viewport.x = 0.0f;
    _viewport.y = 0.0f;
    _viewport.width = static_cast<float>(extent.width);
    _viewport.height = static_cast<float>(extent.height);
    _viewport.minDepth = 0.0f;
    _viewport.maxDepth = 1.0f;

    _scissor.offset = {0, 0};
    _scissor.extent = extent;
    return *this;
}

RenderPipeline& RenderPipeline::SetViewport(
    float x,
    float y,
    float width,
    float height,
    float minDepth/* = 0.0f */,
    float maxDepth/* = 1.0f*/
)
{
    _viewport.x = x;
    _viewport.y = y;
    _viewport.width = width;
    _viewport.height = height;
    _viewport.minDepth = minDepth;
    _viewport.maxDepth = maxDepth;
    
    // Set matching scissor rectangle
    _scissor.offset.x = static_cast<int32_t>(x);
    _scissor.offset.y = static_cast<int32_t>(y);
    _scissor.extent.width = static_cast<uint32_t>(width);
    _scissor.extent.height = static_cast<uint32_t>(height);
    
    return *this;
}

RenderPipeline& RenderPipeline::SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    _scissor.offset.x = x;
    _scissor.offset.y = y;
    _scissor.extent.width = width;
    _scissor.extent.height = height;
    return *this;
}

RenderPipeline& RenderPipeline::SetPipelineLayout(const VkPipelineLayout pipelineLayout)
{
    _pipelineLayout = pipelineLayout;
    return *this;
}

RenderPipeline& RenderPipeline::SetVertexInput(const VkPipelineVertexInputStateCreateInfo& vertexInput)
{
    _vertexInputInfo = vertexInput;
    return *this;
}

RenderPipeline& RenderPipeline::AddShader(const Shader& shader)
{
    _shaderStages.push_back(shader.GetCreateInfo());
    return *this;
}

RenderPipeline& RenderPipeline::AddShaders(const std::vector<std::reference_wrapper<const Shader>>& shaders)
{
    for (const auto& shader : shaders)
    {
        _shaderStages.push_back(shader.get().GetCreateInfo());
    }
    return *this;
}

RenderPipeline& RenderPipeline::SetTopology(VkPrimitiveTopology topology)
{
    _inputAssembly.topology = topology;
    return *this;
}

RenderPipeline& RenderPipeline::SetPolygonMode(VkPolygonMode mode)
{
    _rasterizer.polygonMode = mode;
    return *this;
}

RenderPipeline& RenderPipeline::SetCullMode(
    VkCullModeFlags cullMode,
    VkFrontFace frontFace/* = VK_FRONT_FACE_COUNTER_CLOCKWISE*/
)
{
    _rasterizer.cullMode = cullMode;
    _rasterizer.frontFace = frontFace;
    return *this;
}

RenderPipeline& RenderPipeline::SetDepthTest(
    bool enable,
    bool write/* = true*/,
    VkCompareOp compareOp/* = VK_COMPARE_OP_LESS*/
)
{
    _depthStencil.depthTestEnable = enable ? VK_TRUE : VK_FALSE;
    _depthStencil.depthWriteEnable = write ? VK_TRUE : VK_FALSE;
    _depthStencil.depthCompareOp = enable ? compareOp : VK_COMPARE_OP_ALWAYS;
    return *this;
}

RenderPipeline& RenderPipeline::EnableAlphaBlending()
{
    _colorBlendAttachment.blendEnable = VK_TRUE;
    _colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    _colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    _colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    _colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    _colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    _colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    return *this;
}

RenderPipeline& RenderPipeline::DisableBlending()
{
    _colorBlendAttachment.blendEnable = VK_FALSE;
    return *this;
}

/// @brief Creates the actual Vulkan pipeline
VkPipeline RenderPipeline::GetPipeline()
{
    ValidateState();

    // Viewport state
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // Dynamic state for viewport and scissor (common practice)
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // Color blending state
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &_colorBlendAttachment;

    // Pipeline create info
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(_shaderStages.size());
    pipelineInfo.pStages = _shaderStages.data();
    pipelineInfo.pVertexInputState = &_vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &_inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.pRasterizationState = &_rasterizer;
    pipelineInfo.pMultisampleState = &_multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &_depthStencil;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = _renderPass;
    pipelineInfo.subpass = 0;

    VkPipeline pipeline;
    VkResult result = vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline: " + std::to_string(result));
    }

    return pipeline;
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

void RenderPipeline::SetDefaultValues()
{
    // Vertex input - empty by default (must be set by user)
    _vertexInputInfo = VkExtVertexInputStateCreateInfo();

    // Input assembly - triangle list (most common)
    _inputAssembly = VkExtInputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    // Rasterization - filled triangles, no culling, counter-clockwise front face
    _rasterizer = VkExtRasterizationStateCreateInfo(
        VK_POLYGON_MODE_FILL,           // Fill triangles (not wireframe)
        VK_CULL_MODE_BACK_BIT,          // Cull back faces (standard for 3D)
        VK_FRONT_FACE_COUNTER_CLOCKWISE // Standard winding order
    );

    // Multisampling - disabled (most common for performance)
    _multisampling = VkExtMultisamplingStateCreateInfo();

    // Color blending - no blending, write all color channels
    _colorBlendAttachment = VkExtColorBlendAttachmentState();
    _colorBlendAttachment.blendEnable = VK_FALSE; // Opaque rendering
    _colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
                                          VK_COLOR_COMPONENT_G_BIT | 
                                          VK_COLOR_COMPONENT_B_BIT | 
                                          VK_COLOR_COMPONENT_A_BIT;

    // Depth testing - enabled with less-than comparison
    _depthStencil = VkExtDepthStencilCreateInfo(
        true,                    // Enable depth testing
        true,                    // Enable depth writing  
        VK_COMPARE_OP_LESS       // Standard depth comparison
    );

    // Viewport and scissor - will be zero-initialized
    // These MUST be set by the user since we don't know window size
    _viewport = {};
    _scissor = {};
}

void RenderPipeline::ValidateState()
{
    if (_device == VK_NULL_HANDLE) throw std::runtime_error("Device not set");
    if (_renderPass == VK_NULL_HANDLE) throw std::runtime_error("Render pass not set");
    if (_pipelineLayout == VK_NULL_HANDLE) throw std::runtime_error("Pipeline layout not set");
    if (_shaderStages.empty()) throw std::runtime_error("No shader stages added");
    if (_viewport.width == 0 || _viewport.height == 0) throw std::runtime_error("Viewport not set - call SetViewport()");
}

} // namespace velecs::graphics
