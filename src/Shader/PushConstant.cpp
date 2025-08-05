/// @file    PushConstant.cpp
/// @author  Matthew Green
/// @date    2025-08-05 11:33:21
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/PushConstant.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

PushConstant::PushConstant(const size_t typeHash, const size_t typeSize, const VkPushConstantRange range)
    : _typeHash(typeHash), _typeSize(typeSize), _range(range)
{
    _data.resize(_typeSize);
}

} // namespace velecs::graphics
