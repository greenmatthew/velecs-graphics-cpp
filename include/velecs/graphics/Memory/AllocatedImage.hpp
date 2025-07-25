/// @file    AllocatedImage.hpp
/// @author  Matthew Green
/// @date    2025-07-12 11:59:15
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

/// @struct AllocatedImage
/// @brief Brief description.
///
/// Rest of description.
struct AllocatedImage {
public:
    // Enums

    // Public Fields

    VkImage image{VK_NULL_HANDLE};
    VkImageView imageView{VK_NULL_HANDLE};
    VmaAllocation allocation{VK_NULL_HANDLE};
    VkExtent3D imageExtent{};
    VkFormat imageFormat{VK_FORMAT_UNDEFINED};

    // Constructors and Destructors

    /// @brief Default constructor.
    AllocatedImage() = default;

    /// @brief Default deconstructor.
    ~AllocatedImage() = default;

    // Public Methods

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
