/// @file    Material.hpp
/// @author  Matthew Green
/// @date    2025-07-13 13:51:41
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <memory>

namespace velecs::graphics {

/// @class Material
/// @brief Brief description.
///
/// Rest of description.
class Material {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    Material() = default;

    /// @brief Default deconstructor.
    ~Material() = default;

    // Public Methods

    // Create a material
    // It should use ShaderReflector to determine what fields are allowed.
    static std::shared_ptr<Material> Create();

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
