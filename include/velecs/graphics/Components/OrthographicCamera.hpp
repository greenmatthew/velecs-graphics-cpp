/// @file    OrthographicCamera.hpp
/// @author  Matthew Green
/// @date    2025-07-21 18:13:12
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Components/Camera.hpp"

#include "velecs/graphics/Rect.hpp"

namespace velecs::graphics {

/// @class OrthographicCamera
/// @brief Brief description.
///
/// Rest of description.
class OrthographicCamera : public Camera {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    OrthographicCamera() = default;

    /// @brief Default deconstructor.
    ~OrthographicCamera() = default;

    // Public Methods

    Rect GetProjectionSize() const { return _projectionSize; }
    void SetProjectionSize(const Rect& projectionSize) { _projectionSize = projectionSize; MarkProjectionDirty(); }

protected:
    // Protected Fields

    // Protected Methods

    virtual Mat4 CalculateProjectionMatrix() const override;

private:
    // Private Fields

    Rect _projectionSize{0, 0, 1920, 1080};

    // Private Methods
};

} // namespace velecs::graphics
