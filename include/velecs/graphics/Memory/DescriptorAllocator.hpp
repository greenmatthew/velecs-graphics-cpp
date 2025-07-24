/// @file    DescriptorAllocator.hpp
/// @author  Matthew Green
/// @date    2025-07-24 15:10:04
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace velecs::graphics {

/// @class DescriptorAllocator
/// @brief Brief description.
///
/// Rest of description.
class DescriptorAllocator {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    DescriptorAllocator() = default;

    /// @brief Default deconstructor.
    ~DescriptorAllocator() = default;

    // Public Methods

    struct PoolSizeRatio{
        VkDescriptorType type;
        uint32_t ratio;
    };

    void InitPool(const VkDevice device, const uint32_t maxSets, const std::vector<PoolSizeRatio>& poolRatios);
    void ClearDescriptors(const VkDevice device);
    void DestroyPool(const VkDevice device);
    VkDescriptorSet Allocate(const VkDevice device, const VkDescriptorSetLayout layout);

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    VkDescriptorPool _pool{nullptr};

    // Private Methods
};

} // namespace velecs::graphics
