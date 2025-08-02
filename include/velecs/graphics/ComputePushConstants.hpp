/// @file    ComputePushConstants.hpp
/// @author  Matthew Green
/// @date    2025-07-31 12:34:20
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Color32.hpp"

#include <velecs/math/Vec4.hpp>
using velecs::math::Vec4;

namespace velecs::graphics {

/// @struct ComputePushConstants
/// @brief Brief description.
///
/// Rest of description.
struct ComputePushConstants {
    Vec4 data1{Vec4::ZERO};
    Vec4 data2{Vec4::ZERO};
    Vec4 data3{Vec4::ZERO};
    Vec4 data4{Vec4::ZERO};

    static_assert(sizeof(Color32) == 4);
    static_assert(sizeof(Vec4) == 16);
};

} // namespace velecs::graphics
