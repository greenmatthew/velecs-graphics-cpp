/// @file    RenderPipelineBuilder.cpp
/// @author  Matthew Green
/// @date    2025-07-18 15:43:18
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/RenderPipelineBuilder.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

RenderPipelineBuilder& RenderPipelineBuilder::SetShaders(const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages)
{
    _shaderStages = shaderStages;
    return *this;
}

RenderPipelineBuilder& RenderPipelineBuilder::SetVertexInput(const VkPipelineVertexInputStateCreateInfo& vertexInput)
{
    _vertexInputInfo = vertexInput;

    return *this;
}

RenderPipelineBuilder& RenderPipelineBuilder::SetTopology(VkPrimitiveTopology topology)
{
    _inputAssembly.topology = topology;
    _inputAssembly.primitiveRestartEnable = VK_FALSE;

    return *this;
}

RenderPipelineBuilder& RenderPipelineBuilder::SetPolygonMode(VkPolygonMode mode)
{
    _rasterizer.polygonMode = mode;
    _rasterizer.lineWidth = 1.0f;

    return *this;
}

RenderPipelineBuilder& RenderPipelineBuilder::SetCullMode(
    VkCullModeFlags cullMode,
    VkFrontFace frontFace/* = VK_FRONT_FACE_COUNTER_CLOCKWISE*/
)
{
    _rasterizer.cullMode = cullMode;
    _rasterizer.frontFace = frontFace;

    return *this;
}

RenderPipelineBuilder& RenderPipelineBuilder::SetMultisamplingNone()
{
    _multisampling.sampleShadingEnable = VK_FALSE;
    // multisampling defaulted to no multisampling (1 sample per pixel)
    _multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    _multisampling.minSampleShading = 1.0f;
    _multisampling.pSampleMask = nullptr;
    // no alpha to coverage either
    _multisampling.alphaToCoverageEnable = VK_FALSE;
    _multisampling.alphaToOneEnable = VK_FALSE;

    return *this;
}

RenderPipelineBuilder& RenderPipelineBuilder::EnableAlphaBlending()
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

RenderPipelineBuilder& RenderPipelineBuilder::DisableBlending()
{
    // Default write mask
    _colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT
        ;
    // No blending
    _colorBlendAttachment.blendEnable = VK_FALSE;

    return *this;
}

RenderPipelineBuilder& RenderPipelineBuilder::SetColorAttachmentFormat(const VkFormat format)
{
    _colorAttachmentFormat = format;
    // Connect the format to the renderInfo structure
    _renderInfo.colorAttachmentCount = 1;
    _renderInfo.pColorAttachmentFormats = &_colorAttachmentFormat;

    return *this;
}

RenderPipelineBuilder& RenderPipelineBuilder::SetDepthFormat(const VkFormat format)
{
    _renderInfo.depthAttachmentFormat = format;

    return *this;
}

RenderPipelineBuilder& RenderPipelineBuilder::DisableDepthTest()
{
    _depthStencil.depthTestEnable = VK_FALSE;
    _depthStencil.depthWriteEnable = VK_FALSE;
    _depthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
    _depthStencil.depthBoundsTestEnable = VK_FALSE;
    _depthStencil.stencilTestEnable = VK_FALSE;
    _depthStencil.front = {};
    _depthStencil.back = {};
    _depthStencil.minDepthBounds = 0.f;
    _depthStencil.maxDepthBounds = 1.f;

    return *this;
}

RenderPipelineBuilder& RenderPipelineBuilder::SetDepthTest(
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

void RenderPipelineBuilder::Clear()
{
    _shaderStages.clear();

    _vertexInputInfo = {};
    _vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    _inputAssembly = {};
    _inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    _rasterizer = {};
    _rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    _colorBlendAttachment = {};

    _multisampling = {};
    _multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    _pipelineLayout = {};

    _depthStencil = {};
    _depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    _renderInfo = {};
    _renderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
}

// Protected Fields

// Protected Methods

VkPipeline RenderPipelineBuilder::CreatePipeline()
{
    // Make viewport state from our stored viewport and scissor.
    // At the moment, we wont support multiple viewports or scissors.
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &_colorBlendAttachment;

    // Dynamic state for viewport and scissor (common practice)
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // Pipeline create info
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = &_renderInfo;
    pipelineInfo.flags = 0;
    pipelineInfo.stageCount = static_cast<uint32_t>(_shaderStages.size());;
    pipelineInfo.pStages = _shaderStages.data();
    pipelineInfo.pVertexInputState = &_vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &_inputAssembly;
    pipelineInfo.pTessellationState = nullptr;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &_rasterizer;
    pipelineInfo.pMultisampleState = &_multisampling;
    pipelineInfo.pDepthStencilState = &_depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = nullptr;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = 0;

    VkPipeline pipeline;
    VkResult result = vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline: " + std::to_string(result));
    }

    return pipeline;
}

// Private Fields

// Private Methods

} // namespace velecs::graphics
