/// @file    TestVertex.hpp
/// @author  Matthew Green
/// @date    2025-08-06 16:31:22
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Color32.hpp"

#include <velecs/math/Vec2.hpp>
#include <velecs/math/Vec3.hpp>
#include <velecs/math/Vec4.hpp>

using velecs::math::Vec2;
using velecs::math::Vec3;
using velecs::math::Vec4;


namespace velecs::graphics {

/// @struct TestVertex
/// @brief Brief description.
///
/// Rest of description.
struct TestVertex {
    Vec3 pos{Vec3::ZERO};
    Vec2 uv{Vec2::ZERO};
    Vec3 normal{Vec3::ZERO};
    Vec4 color{Color32::MAGENTA};

    static_assert(sizeof(Vec2) == 8);
    static_assert(sizeof(Vec3) == 12);
    static_assert(sizeof(Vec4) == 16);
};

} // namespace velecs::graphics
