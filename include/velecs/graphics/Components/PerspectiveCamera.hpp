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

    void SetVerticalFov(const float vFov);
    void SetHorizontalFov(const float hFov);
    void SetAspectRatio(const float aspectRatio);
    void SetNearPlane(const float near);
    void SetFarPlane(const float far);

protected:
    // Protected Fields

    // Protected Methods

    virtual Mat4 CalculateProjectionMatrix() const override;

private:
    // Private Fields

    float _vFov{80.0f};
    float _aspectRatio{16.0f / 9.0f};
    float _near{0.1f};
    float _far{1000.0f};

    // Private Methods
};

} // namespace velecs::graphics
