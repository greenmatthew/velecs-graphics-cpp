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
#include <vector>
#include <functional>
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

    // Constructors and Destructors
    
    /// @brief Constructor access key to enforce factory method usage
    class ConstructorKey {
        friend struct AllocatedBuffer;
        ConstructorKey() = default;
    };
    
    /// @brief Constructor for internal use (use factory methods instead)
    inline AllocatedBuffer(VkBuffer buffer, VmaAllocation allocation, VmaAllocator allocator, ConstructorKey)
        : _buffer(buffer), _allocation(allocation), _allocator(allocator) {}
    
    /// @brief Destructor - automatically cleans up resources
    ~AllocatedBuffer() { Destroy(); }

    // Delete copy and move operations to ensure clear ownership semantics
    // Use std::unique_ptr<AllocatedBuffer> for transferable ownership
    AllocatedBuffer(const AllocatedBuffer&) = delete;
    AllocatedBuffer& operator=(const AllocatedBuffer&) = delete;
    AllocatedBuffer(AllocatedBuffer&&) = delete;
    AllocatedBuffer& operator=(AllocatedBuffer&&) = delete;

    // Public Methods

    /// @brief Creates a GPU buffer with immediate upload from CPU data
    /// @param allocator VMA allocator for buffer creation
    /// @param data Pointer to raw data to upload
    /// @param dataSize Total size of data in bytes
    /// @param usage Buffer usage flags (vertex, index, uniform, etc.)
    /// @param immediateSubmit Function to submit commands immediately and wait
    /// @return Unique pointer to created buffer, or nullptr on failure
    static std::unique_ptr<AllocatedBuffer> CreateImmediately(
        VmaAllocator allocator,
        const void* data,
        size_t dataSize,
        VkBufferUsageFlags usage,
        std::function<void(std::function<void(VkCommandBuffer)>)> immediateSubmit
    );

    /// @brief Convenient template wrapper for vector data
    /// @tparam T The type of data in the vector
    /// @param allocator VMA allocator for buffer creation
    /// @param data Vector of data to upload to GPU
    /// @param usage Buffer usage flags (vertex, index, uniform, etc.)
    /// @param immediateSubmit Function to submit commands immediately and wait
    /// @return Unique pointer to created buffer, or nullptr on failure
    template<typename T>
    static std::unique_ptr<AllocatedBuffer> CreateImmediately(
        VmaAllocator allocator,
        const std::vector<T>& data,
        VkBufferUsageFlags usage,
        std::function<void(std::function<void(VkCommandBuffer)>)> immediateSubmit
    )
    {
        if (data.empty()) return nullptr;
        return CreateImmediately(allocator, data.data(), data.size() * sizeof(T), usage, immediateSubmit);
    }

    /// @brief Creates an empty GPU buffer of specified size
    /// @param allocator VMA allocator for buffer creation
    /// @param size Size of buffer in bytes
    /// @param usage Buffer usage flags
    /// @param memoryUsage VMA memory usage pattern (default: GPU_ONLY)
    /// @return Unique pointer to created buffer, or nullptr on failure
    static std::unique_ptr<AllocatedBuffer> CreateEmpty(
        VmaAllocator allocator,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY
    );

    /// @brief Checks if the buffer is valid and ready for use
    /// @return True if buffer and allocation are valid
    inline bool IsValid() const
    { 
        return _buffer != VK_NULL_HANDLE && _allocation != VK_NULL_HANDLE && _allocator != VK_NULL_HANDLE; 
    }

    /// @brief Gets the Vulkan buffer handle
    /// @return The VkBuffer handle
    inline VkBuffer GetBuffer() const { return _buffer; }

    /// @brief Gets the VMA allocation handle  
    /// @return The VmaAllocation handle
    inline VmaAllocation GetAllocation() const { return _allocation; }

    /// @brief Gets the VMA allocator handle
    /// @return The VmaAllocator handle
    inline VmaAllocator GetAllocator() const { return _allocator; }

    /// @brief Gets buffer size in bytes
    /// @return Size of the allocated buffer
    VkDeviceSize GetSize() const;

    /// @brief Maps buffer memory for CPU access (for CPU-accessible buffers only)
    /// @return Pointer to mapped memory, or nullptr on failure
    void* Map();

    /// @brief Unmaps previously mapped buffer memory
    void Unmap();

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    VkBuffer _buffer{VK_NULL_HANDLE};           /// @brief Vulkan buffer handle
    VmaAllocation _allocation{VK_NULL_HANDLE};  /// @brief VMA allocation handle
    VmaAllocator _allocator{VK_NULL_HANDLE};    /// @brief VMA allocator handle

    // Private Methods
    
    /// @brief Destroys the buffer and its allocation
    void Destroy();
};

} // namespace velecs::graphics