/// @file    DescriptorLayoutBuilder.hpp
/// @author  Matthew Green
/// @date    2025-07-24 14:47:05
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

/// @class DescriptorLayoutBuilder
/// @brief Brief description.
///
/// Rest of description.
class DescriptorLayoutBuilder {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    DescriptorLayoutBuilder() = default;

    /// @brief Default deconstructor.
    ~DescriptorLayoutBuilder() = default;

    // Public Methods

    DescriptorLayoutBuilder& AddBinding(const uint32_t binding, const VkDescriptorType type);

    DescriptorLayoutBuilder& Clear();

    VkDescriptorSetLayout Build(
        const VkDevice device,
        const VkShaderStageFlags stageFlags,
        const void* const pNext = nullptr,
        const VkDescriptorSetLayoutCreateFlags flags = 0
    );

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    std::vector<VkDescriptorSetLayoutBinding> _bindings;

    // Private Methods
};

} // namespace velecs::graphics
