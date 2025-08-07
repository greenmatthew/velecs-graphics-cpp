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

#include <velecs/common/Uuid.hpp>
using velecs::common::Uuid;

#include <memory>
#include <optional>

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

    inline bool IsValid() const { return _programHandle.has_value(); }

    void TrySetShaderProgram(const std::string& name)
    {
        // const Uuid uuid = RenderEngine::_rasterPrograms2.TryGetUuid(name);
        // _programHandle = uuid;
    }

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    std::optional<Uuid> _programHandle;

    // Private Methods
};

} // namespace velecs::graphics
