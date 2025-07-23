/// @file    Camera.hpp
/// @author  Matthew Green
/// @date    2025-07-21 18:11:37
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/ecs/Component.hpp>

#include <velecs/math/Mat4.hpp>

namespace velecs::graphics {

/// @class Camera
/// @brief Brief description.
///
/// Rest of description.
class Camera : public velecs::ecs::Component {
public:
    using Mat4 = velecs::math::Mat4;

    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    Camera() = default;

    /// @brief Default deconstructor.
    ~Camera() = default;

    // Public Methods

    const Mat4& GetViewMatrix() const;
    const Mat4& GetProjectionMatrix() const;

protected:
    // Protected Fields

    mutable bool isViewDirty = true;
    mutable bool isProjectionDirty = true;
    mutable Mat4 cachedWorldMatrix{Mat4::IDENTITY};
    mutable Mat4 cachedViewMatrix{Mat4::IDENTITY};
    mutable Mat4 cachedProjectionMatrix{Mat4::IDENTITY};

    // Protected Methods

    inline void MarkViewDirty() { isViewDirty = true; }
    inline void MarkProjectionDirty() { isProjectionDirty = true; }

    virtual Mat4 CalculateViewMatrix() const;
    virtual Mat4 CalculateProjectionMatrix() const = 0;

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
