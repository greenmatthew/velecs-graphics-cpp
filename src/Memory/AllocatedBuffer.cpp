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

#include <iostream>

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

std::unique_ptr<AllocatedBuffer> AllocatedBuffer::TryCreateBuffer(const VmaAllocator allocator, size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
    auto buffer = std::make_unique<AllocatedBuffer>(ConstructorKey{});
    buffer->_allocator = allocator;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.size = allocSize;
    bufferInfo.usage = usage;

    VmaAllocationCreateInfo vmaAllocInfo{};
    vmaAllocInfo.usage = memoryUsage;
    vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    const VkResult result = vmaCreateBuffer(
        buffer->_allocator,
        &bufferInfo,
        &vmaAllocInfo,
        &buffer->buffer,
        &buffer->_allocation,
        &buffer->_allocationInfo
    );
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create buffer: " << result << std::endl;
        return nullptr;
    }

    return buffer;
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

void AllocatedBuffer::Cleanup()
{
    if (_allocator != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(_allocator, buffer, _allocation);
        _allocator = VK_NULL_HANDLE;
        buffer = VK_NULL_HANDLE;
        _allocation = VK_NULL_HANDLE;
        _allocationInfo = {};
    }
}

} // namespace velecs::graphics
