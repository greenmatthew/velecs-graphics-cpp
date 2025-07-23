/// @file    OrthographicCamera.cpp
/// @author  Matthew Green
/// @date    2025-07-21 18:41:41
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Components/OrthographicCamera.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

// Protected Fields

// Protected Methods

OrthographicCamera::Mat4 OrthographicCamera::CalculateProjectionMatrix() const
{
    return Mat4::FromOrthographic(_viewport.GetWidth(), _viewport.GetHeight(), _near, _far);
}

// Private Fields

// Private Methods

} // namespace velecs::graphics
