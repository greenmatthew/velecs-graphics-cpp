/// @file    RasterizationShaderProgram.cpp
/// @author  Matthew Green
/// @date    2025-07-14 13:46:15
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/ShaderPrograms/RasterizationShaderProgram.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

bool RasterizationShaderProgram::IsComplete() const
{
    // Must have vertex and fragment shaders
    if (!vert || !frag) return false;
    
    // If tessellation is used, must have both control and evaluation shaders
    if ((tesc != nullptr) != (tese != nullptr)) return false;
    
    return true;
}

size_t RasterizationShaderProgram::GetStageCount() const
{
    size_t count = 0;
    if (vert) ++count;
    if (frag) ++count;
    if (geom) ++count;
    if (tesc) ++count;
    if (tese) ++count;
    return count;
}

// Protected Fields

// Protected Methods

bool RasterizationShaderProgram::ValidateShaders() const
{
    if (vert && !vert->IsValid()) return false;
    if (frag && !frag->IsValid()) return false;
    if (geom && !geom->IsValid()) return false;
    if (tesc && !tesc->IsValid()) return false;
    if (tese && !tese->IsValid()) return false;
    
    return true;
}

// Private Fields

// Private Methods

} // namespace velecs::graphics
