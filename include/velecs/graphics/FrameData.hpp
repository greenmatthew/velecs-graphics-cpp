/// @file    FrameData.hpp
/// @author  Matthew Green
/// @date    2025-07-24 20:04:25
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Memory/DeletionQueue.hpp"

#include <vulkan/vulkan_core.h>

namespace velecs::graphics {

/// @struct FrameData
/// @brief Brief description.
///
/// Rest of description.
struct FrameData {
public:
    // Enums

    // Public Fields

    VkCommandPool commandPool{nullptr};
	VkCommandBuffer mainCommandBuffer{nullptr};

    VkSemaphore swapchainSemaphore{nullptr};

    VkFence renderFence{nullptr};

    DeletionQueue deletionQueue;

    // Constructors and Destructors

    /// @brief Default constructor.
    FrameData() = default;

    /// @brief Default deconstructor.
    ~FrameData() = default;

    // Public Methods

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
