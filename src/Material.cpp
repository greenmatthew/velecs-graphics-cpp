/// @file    Material.cpp
/// @author  Matthew Green
/// @date    2025-08-07 11:40:19
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Material.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

std::shared_ptr<Material> Material::Create()
{
    return std::make_shared<Material>();
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::graphics
