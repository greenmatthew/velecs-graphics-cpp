/// @file    ObjectUniforms.hpp
/// @author  Matthew Green
/// @date    2025-07-23 17:31:46
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/math/Mat4.hpp>
using velecs::math::Mat4;

#include "velecs/graphics/Color32.hpp"

namespace velecs::graphics {

/// @struct ObjectUniforms
/// @brief Brief description.
///
/// Rest of description.
struct ObjectUniforms {
    Mat4 worldMat;
    Color32 color;
    uint8_t padding[12];

    static_assert(sizeof(Mat4) == 64);
    static_assert(sizeof(Color32) == 4);
};

} // namespace velecs::graphics
