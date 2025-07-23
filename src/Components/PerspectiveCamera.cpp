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

#include "velecs/common/Exceptions.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

void PerspectiveCamera::SetVerticalFovRad(const float vFovRad)
{
    _vFovRad = vFovRad;
    MarkProjectionDirty();
}

void PerspectiveCamera::SetHorizontalFovRad(const float hFovRad)
{
    float vFovRad = HorizontalToVerticalFovRad(hFovRad, GetAspectRatio());
    SetVerticalFovRad(vFovRad);
}

void PerspectiveCamera::SetAspectRatio(const float aspectRatio)
{
    _aspectRatio = aspectRatio;
    MarkProjectionDirty();
}

// Protected Fields

// Protected Methods

PerspectiveCamera::Mat4 PerspectiveCamera::CalculateProjectionMatrix() const
{
    return Mat4::FromPerspectiveRad(GetVerticalFovRad(), GetAspectRatio(), GetNearPlane(), GetFarPlane());
}

// Private Fields

// Private Methods

/// @brief Converts from any arbitrary 2D FOV to the opposite FOV
/// @param fovRad FOV in radians.
/// @param aspectRatio Aspect ratio (width / height or height / width)
/// @return Opposite FOV in radians.
float ConvertFovRad(float fovRad, float aspectRatio)
{
    if (fovRad < std::numeric_limits<float>::epsilon())
    {
        throw std::invalid_argument("FOV cannot be less than or equal to 0.");
    }

    if (aspectRatio < std::numeric_limits<float>::epsilon())
    {
        throw std::invalid_argument("Aspect ratio cannot be less than or equal to 0.");
    }

    const float halfFovRad = fovRad * 0.5f;
    const float t = std::tan(halfFovRad);
    const float halfNewFovRad = std::atan(t * aspectRatio); // Note: multiplication by aspect ratio
    return 2.0f * halfNewFovRad;
}

float PerspectiveCamera::VerticalToHorizontalFovRad(float vFovRad, float aspectRatio)
{
    return ConvertFovRad(vFovRad, aspectRatio);
}

float PerspectiveCamera::HorizontalToVerticalFovRad(float hFovRad, float aspectRatio)
{
    return ConvertFovRad(hFovRad, 1.0f / aspectRatio);
}

} // namespace velecs::graphics
