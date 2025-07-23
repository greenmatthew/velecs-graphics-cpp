/// @file    Camera.cpp
/// @author  Matthew Green
/// @date    2025-07-21 18:37:03
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Components/Camera.hpp"

#include <velecs/ecs/Components/Transform.hpp>
using namespace velecs::ecs;

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

const Camera::Mat4& Camera::GetViewMatrix() const
{
    auto& transform = GetOwner().GetTransform();
    Mat4 currentWorldMatrix = transform.GetWorldMatrix();

    // Check if we need to recalculate (transform changed or manually marked dirty)
    if (isViewDirty || currentWorldMatrix.FastNotEqual(cachedWorldMatrix))
    {
        cachedWorldMatrix = currentWorldMatrix;
        cachedViewMatrix = CalculateViewMatrix();
        isViewDirty = false;
    }

    return cachedViewMatrix;
}

const Camera::Mat4& Camera::GetProjectionMatrix() const
{
    if (isProjectionDirty)
    {
        cachedProjectionMatrix = CalculateProjectionMatrix();
        isProjectionDirty = false;
    }

    return cachedProjectionMatrix;
}

// Protected Fields

// Protected Methods

Camera::Mat4 Camera::CalculateViewMatrix() const
{
    return cachedWorldMatrix.Inverse();
}

// Private Fields

// Private Methods

} // namespace velecs::graphics
