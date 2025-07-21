/// @file    MeshRenderer.hpp
/// @author  Matthew Green
/// @date    2025-07-21 16:17:38
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/ecs/Component.hpp>

#include "velecs/graphics/Mesh.hpp"
#include "velecs/graphics/Material.hpp"

namespace velecs::graphics {

/// @class MeshRenderer
/// @brief Brief description.
///
/// Rest of description.
class MeshRenderer : public velecs::ecs::Component {
public:
    // Enums

    // Public Fields

    std::shared_ptr<Mesh> mesh;
    // std::shared_ptr<Material> mat;

    // Constructors and Destructors

    /// @brief Default constructor.
    MeshRenderer() = default;

    /// @brief Default deconstructor.
    ~MeshRenderer() = default;

    // Public Methods

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
