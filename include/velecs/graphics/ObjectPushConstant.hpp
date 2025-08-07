/// @file    ObjectPushConstant.hpp
/// @author  Matthew Green
/// @date    2025-08-07 10:45:31
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/math/Mat4.hpp>
using velecs::math::Mat4;

#include <vulkan/vulkan_core.h>

namespace velecs::graphics {

/// @struct ObjectPushConstant
/// @brief Brief description.
///
/// Rest of description.
struct ObjectPushConstant {
    Mat4 worldMatrix;
    VkDeviceAddress vertexBuffer;
};

} // namespace velecs::graphics
