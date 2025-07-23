/// @file    PerspectiveCamera.hpp
/// @author  Matthew Green
/// @date    2025-07-21 18:12:17
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Components/Camera.hpp"

#include <velecs/math/Consts.hpp>

#include <cmath>

namespace velecs::graphics {

/// @class PerspectiveCamera
/// @brief Brief description.
///
/// Rest of description.
class PerspectiveCamera : public Camera {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    PerspectiveCamera() = default;

    /// @brief Default deconstructor.
    ~PerspectiveCamera() = default;

    // Public Methods

    inline float GetVerticalFovRad() const { return _vFovRad; }

    inline float GetVerticalFovDeg() const { return _vFovRad * math::RAD_TO_DEG; }

    inline float GetHorizontalFovRad() const { return VerticalToHorizontalFovRad(GetVerticalFovRad(), GetAspectRatio()); }

    inline float GetHorizontalFovDeg() const { return GetHorizontalFovRad() * math::RAD_TO_DEG; }

    inline float GetAspectRatio() const { return _aspectRatio; }

    void SetVerticalFovRad(const float vFovRad);

    inline void SetVerticalFovDeg(const float vFovDeg) { SetVerticalFovRad(vFovDeg * math::DEG_TO_RAD); }

    void SetHorizontalFovRad(const float hFovRad);

    inline void SetHorizontalFovDeg(const float hFovDeg) { SetHorizontalFovRad(hFovDeg * math::DEG_TO_RAD); }

    void SetAspectRatio(const float aspectRatio);

protected:
    // Protected Fields

    // Protected Methods

    virtual Mat4 CalculateProjectionMatrix() const override;

private:
    // Private Fields

    float _vFovRad{80.0f};
    float _aspectRatio{16.0f / 9.0f};

    // Private Methods

    /// @brief Converts vertical FOV to horizontal FOV.
    /// @param vFovRad Vertical FOV in radians.
    /// @param aspectRatio Aspect ratio (width / height).
    /// @return Horizontal FOV in radians.
    static float VerticalToHorizontalFovRad(float vFovRad, float aspectRatio);

    /// @brief Converts horizontal FOV to vertical FOV.
    /// @param hFovRad Horizontal FOV in radians.
    /// @param aspectRatio Aspect ratio (width / height).
    /// @return Vertical FOV in radians.
    static float HorizontalToVerticalFovRad(float hFovRad, float aspectRatio);
};

} // namespace velecs::graphics
