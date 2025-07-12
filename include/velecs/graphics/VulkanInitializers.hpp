/// @file    VulkanInitializers.hpp
/// @author  Matthew Green
/// @date    2025-07-12 12:03:16
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <vulkan/vulkan_core.h>

namespace velecs::graphics {

VkCommandPoolCreateInfo VkExtCommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);

VkCommandBufferAllocateInfo VkExtCommandBufferAllocateInfo(VkCommandPool pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

VkPipelineShaderStageCreateInfo VkExtPipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shaderModule);

VkPipelineVertexInputStateCreateInfo VkExtVertexInputStateCreateInfo();

VkPipelineInputAssemblyStateCreateInfo VkExtInputAssemblyCreateInfo(VkPrimitiveTopology topology);

VkPipelineRasterizationStateCreateInfo VkExtRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace);

VkPipelineMultisampleStateCreateInfo VkExtMultisamplingStateCreateInfo();

VkPipelineColorBlendAttachmentState VkExtColorBlendAttachmentState();

VkPipelineLayoutCreateInfo VkExtPipelineLayoutCreateInfo();

VkFenceCreateInfo VkExtFenceCreateInfo(VkFenceCreateFlags flags = 0);

VkSemaphoreCreateInfo VkExtSemaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);

VkImageCreateInfo VkExtImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);

VkImageViewCreateInfo VkExtImageviewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);

VkPipelineDepthStencilStateCreateInfo VkExtDepthStencilCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);

VkCommandBufferBeginInfo VkExtCommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);

VkSubmitInfo VkExtSubmitInfo(VkCommandBuffer* cmd);

} // namespace velecs::graphics
