/// @file    AllocatedBuffer.cpp
/// @author  Matthew Green
/// @date    2025-07-21 13:05:44
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Memory/AllocatedBuffer.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

std::unique_ptr<AllocatedBuffer> AllocatedBuffer::CreateImmediately(
    VmaAllocator allocator,
    const void* data,
    size_t dataSize,
    VkBufferUsageFlags usage,
    std::function<void(std::function<void(VkCommandBuffer)>)> immediateSubmit
)
{
    if (!data || dataSize == 0) return nullptr;

    // Create staging buffer (CPU accessible)
    VkBufferCreateInfo stagingBufferInfo{};
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.size = dataSize;
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    
    VmaAllocationCreateInfo stagingAllocInfo{};
    stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    
    VkBuffer stagingBuffer;
    VmaAllocation stagingAllocation;
    VkResult result = vmaCreateBuffer(
        allocator,
        &stagingBufferInfo,
        &stagingAllocInfo,
        &stagingBuffer,
        &stagingAllocation,
        nullptr
    );
    if (result != VK_SUCCESS) return nullptr;
    
    // Copy data to staging buffer
    void* mappedData;
    vmaMapMemory(allocator, stagingAllocation, &mappedData);
    memcpy(mappedData, data, dataSize);
    vmaUnmapMemory(allocator, stagingAllocation);
    
    // Create GPU buffer
    VkBufferCreateInfo gpuBufferInfo{};
    gpuBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    gpuBufferInfo.size = dataSize;
    gpuBufferInfo.usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    
    VmaAllocationCreateInfo gpuAllocInfo{};
    gpuAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    
    VkBuffer gpuBuffer;
    VmaAllocation gpuAllocation;
    result = vmaCreateBuffer(
        allocator,
        &gpuBufferInfo,
        &gpuAllocInfo,
        &gpuBuffer,
        &gpuAllocation,
        nullptr
    );
    if (result != VK_SUCCESS) {
        // Cleanup staging buffer before returning
        vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
        return nullptr;
    }
    
    // Copy from staging to GPU buffer
    immediateSubmit([=](VkCommandBuffer cmd) {
        VkBufferCopy copy{};
        copy.size = dataSize;
        vkCmdCopyBuffer(cmd, stagingBuffer, gpuBuffer, 1, &copy);
    });
    
    // Cleanup staging buffer
    vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
    
    // Return the created buffer
    return std::make_unique<AllocatedBuffer>(gpuBuffer, gpuAllocation, allocator, ConstructorKey{});
}

std::unique_ptr<AllocatedBuffer> AllocatedBuffer::CreateEmpty(
    VmaAllocator allocator,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VmaMemoryUsage memoryUsage/* = VMA_MEMORY_USAGE_GPU_ONLY*/
)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    
    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = memoryUsage;
    
    VkBuffer buffer;
    VmaAllocation allocation;
    VkResult result = vmaCreateBuffer(
        allocator,
        &bufferInfo,
        &allocInfo,
        &buffer,
        &allocation,
        nullptr
    );
    
    if (result != VK_SUCCESS) {
        return nullptr;
    }
    
    return std::make_unique<AllocatedBuffer>(buffer, allocation, allocator, ConstructorKey{});
}

VkDeviceSize AllocatedBuffer::GetSize() const
{
    if (!IsValid()) return 0;
    
    VmaAllocationInfo allocInfo;
    vmaGetAllocationInfo(_allocator, _allocation, &allocInfo);
    return allocInfo.size;
}

void* AllocatedBuffer::Map()
{
    if (!IsValid()) return nullptr;
    
    void* data;
    VkResult result = vmaMapMemory(_allocator, _allocation, &data);
    return (result == VK_SUCCESS) ? data : nullptr;
}

void AllocatedBuffer::Unmap()
{
    if (IsValid())
    {
        vmaUnmapMemory(_allocator, _allocation);
    }
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

void AllocatedBuffer::Destroy()
{
    if (IsValid())
    {
        vmaDestroyBuffer(_allocator, _buffer, _allocation);
        _buffer = VK_NULL_HANDLE;
        _allocation = VK_NULL_HANDLE;
        _allocator = VK_NULL_HANDLE;
    }
}

} // namespace velecs::graphics
