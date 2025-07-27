/// @file    ComputeShaderProgram.cpp
/// @author  Matthew Green
/// @date    2025-07-25 14:58:43
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/ShaderPrograms/ComputeShaderProgram.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

bool ComputeShaderProgram::IsComplete() const
{
    return comp ? true : false;
}

size_t ComputeShaderProgram::GetStageCount() const
{
    return comp ? 1 : 0;
}

// Protected Fields

// Protected Methods

bool ComputeShaderProgram::ValidateShaders() const
{
    return comp && comp->IsValid();
}

// Private Fields

// Private Methods

} // namespace velecs::graphics
