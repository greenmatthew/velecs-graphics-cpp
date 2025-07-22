/// @file    PerspectiveCamera.cpp
/// @author  Matthew Green
/// @date    2025-07-21 18:41:10
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Components/PerspectiveCamera.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

// Protected Fields

// Protected Methods

PerspectiveCamera::Mat4 PerspectiveCamera::CalculateProjectionMatrix() const
{
    return Mat4::FromPerspective(_vFov, _aspectRatio, _near, _far);
}

// Private Fields

// Private Methods

} // namespace velecs::graphics
