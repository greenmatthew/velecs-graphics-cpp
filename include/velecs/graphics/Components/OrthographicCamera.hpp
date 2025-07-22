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

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    float _near{0.1f};
    float _far{1000.0f};

    // Private Methods
};

} // namespace velecs::graphics
