/// @file    AllocatedBuffer.hpp
/// @author  Matthew Green
/// @date    2025-07-12 11:58:26
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <vulkan/vulkan_core.h>

#include <vma/vk_mem_alloc.h>

#include <memory>
#include <optional>

namespace velecs::graphics {

/// @struct AllocatedBuffer
/// @brief Represents a Vulkan buffer with its associated VMA allocation.
///
/// Provides static factory methods for creating buffers with different strategies
/// (immediate upload, background upload, etc.) while encapsulating the complexity
/// of staging buffer creation and memory management.
struct AllocatedBuffer {
public:
    // Enums

    // Public Fields

    VkBuffer buffer{VK_NULL_HANDLE}; /// @brief Vulkan buffer handle

    // Constructors and Destructors
    
    /// @brief Constructor access key to enforce factory method usage
    class ConstructorKey {
        friend struct AllocatedBuffer;
        ConstructorKey() = default;
    };
    
    /// @brief Constructor for internal use (use factory methods instead)
    inline AllocatedBuffer(ConstructorKey) {}
    
    /// @brief Destructor - automatically cleans up resources
    ~AllocatedBuffer() { Cleanup(); }

    // Delete copy and move operations to ensure clear ownership semantics
    // Use std::unique_ptr<AllocatedBuffer> for transferable ownership
    AllocatedBuffer(const AllocatedBuffer&) = delete;
    AllocatedBuffer& operator=(const AllocatedBuffer&) = delete;
    AllocatedBuffer(AllocatedBuffer&&) = delete;
    AllocatedBuffer& operator=(AllocatedBuffer&&) = delete;

    // Public Methods

    static std::unique_ptr<AllocatedBuffer> TryCreateBuffer(
        const VmaAllocator allocator,
        size_t allocSize,
        VkBufferUsageFlags usage,
        VmaMemoryUsage memoryUsage
    );

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    VmaAllocator _allocator{VK_NULL_HANDLE};
    VmaAllocation _allocation{VK_NULL_HANDLE};  /// @brief VMA allocation handle
    VmaAllocationInfo _allocationInfo;

    // Private Methods
    
    /// @brief Destroys the buffer and its allocation
    void Cleanup();
};

} // namespace velecs::graphics