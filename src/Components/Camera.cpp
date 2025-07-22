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

Camera::Mat4 Camera::GetWorldMatrix() const
{
    return cachedProjectionMatrix;
}

Camera::Mat4 Camera::GetViewMatrix() const
{
    // No easy way to get the transform attached to the same Entity.
    throw std::runtime_error("Function not implemented!");

    // check if last world mat is the same as the current using Mat4::FastEqual
    // if (isViewDirty || transform.GetWorldMatrix().FastEqual(cachedWorldMatrix))
    // {
    //     cachedViewMatrix = CalculateViewMatrix();
    //     isViewDirty = false;
    // }

    // return cachedViewMatrix;
}

Camera::Mat4 Camera::GetProjectionMatrix() const
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

Camera::Mat4 Camera::CalculateWorldMatrix() const
{
    // No easy way to get the transform attached to the same Entity.
    throw std::runtime_error("Function not implemented!");
}

Camera::Mat4 Camera::CalculateViewMatrix() const
{
    return GetWorldMatrix().Inverse();
}

// Private Fields

// Private Methods

} // namespace velecs::graphics
