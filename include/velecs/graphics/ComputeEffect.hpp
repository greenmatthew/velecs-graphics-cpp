/// @file    ComputeEffect.hpp
/// @author  Matthew Green
/// @date    2025-08-02 11:23:43
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/ComputePushConstants.hpp"

#include "vulkan/vulkan_core.h"

#include <string>

namespace velecs::graphics {

/// @struct ComputeEffect
/// @brief Brief description.
///
/// Rest of description.
struct ComputeEffect {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    ComputeEffect() = default;

    /// @brief Default deconstructor.
    ~ComputeEffect() = default;

    // Public Methods

    std::string name;

    VkPipeline pipeline;
    VkPipelineLayout layout;

    ComputePushConstants data;

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
