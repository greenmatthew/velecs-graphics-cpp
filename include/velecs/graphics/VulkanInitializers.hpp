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

#include <string>

namespace velecs::graphics {

VkCommandPoolCreateInfo VkExtCommandPoolCreateInfo(const uint32_t queueFamilyIndex, const VkCommandPoolCreateFlags flags = 0);

VkCommandBufferAllocateInfo VkExtCommandBufferAllocateInfo(
    const VkCommandPool pool,
    const uint32_t count = 1,
    const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY
);

VkFenceCreateInfo VkExtFenceCreateInfo(const VkFenceCreateFlags flags = 0);

VkSemaphoreCreateInfo VkExtSemaphoreCreateInfo(const VkSemaphoreCreateFlags flags = 0);

VkCommandBufferBeginInfo VkExtCommandBufferBeginInfo(const VkCommandBufferUsageFlags flags = 0);

VkImageSubresourceRange VkExtImageSubresourceRange(const VkImageAspectFlags aspectMask);

VkSemaphoreSubmitInfo VkExtSemaphoreSubmitInfo(const VkPipelineStageFlags2 stageMask, const VkSemaphore semaphore);

VkCommandBufferSubmitInfo VkExtCommandBufferSubmitInfo(const VkCommandBuffer cmd);

VkSubmitInfo2 VkExtSubmitInfo2(
    const VkCommandBufferSubmitInfo* cmd,
    const VkSemaphoreSubmitInfo* signalSemaphoreInfo,
    const VkSemaphoreSubmitInfo* waitSemaphoreInfo
);

VkImageCreateInfo VkExtImageCreateInfo(const VkFormat format, const VkExtent3D extent, const VkImageUsageFlags usageFlags);

VkImageViewCreateInfo VkExtImageviewCreateInfo(const VkFormat format, const VkImage image, const VkImageAspectFlags aspectFlags);

VkPipelineShaderStageCreateInfo VkExtPipelineShaderStageCreateInfo(
    const VkShaderStageFlagBits stage,
    const VkShaderModule shaderModule,
    const std::string& entryPoint = "main"
);












VkPipelineVertexInputStateCreateInfo VkExtVertexInputStateCreateInfo();

VkPipelineInputAssemblyStateCreateInfo VkExtInputAssemblyCreateInfo(VkPrimitiveTopology topology);

VkPipelineRasterizationStateCreateInfo VkExtRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace);

VkPipelineMultisampleStateCreateInfo VkExtMultisamplingStateCreateInfo();

VkPipelineColorBlendAttachmentState VkExtColorBlendAttachmentState();

VkPipelineLayoutCreateInfo VkExtPipelineLayoutCreateInfo();

VkPipelineDepthStencilStateCreateInfo VkExtDepthStencilCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);

} // namespace velecs::graphics
