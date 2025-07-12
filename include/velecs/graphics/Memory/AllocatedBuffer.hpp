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

namespace velecs::graphics {

/// @struct AllocatedBuffer
/// @brief Brief description.
///
/// Rest of description.
struct AllocatedBuffer {
public:
    // Enums

    // Public Fields

    VkBuffer _buffer{nullptr};
    VmaAllocation _allocation{nullptr};

    // Constructors and Destructors
    
    /// @brief Default constructor.
    AllocatedBuffer() = default;
    
    /// @brief Default deconstructor.
    ~AllocatedBuffer() = default;

    // Public Methods

    inline bool IsInitialized() const { return _buffer != nullptr && _allocation != nullptr; }

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
