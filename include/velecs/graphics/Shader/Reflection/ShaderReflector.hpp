/// @file    ShaderReflector.hpp
/// @author  Matthew Green
/// @date    2025-07-14 14:08:24
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Shader/Reflection/ShaderReflectionData.hpp"

namespace velecs::graphics {

class Shader;

/// @brief Reflects a shader and extracts its resource information
/// @param shader The shader to reflect
/// @return Reflection data containing all discovered resources
/// @throws std::runtime_error if reflection fails
ShaderReflectionData Reflect(const Shader& shader);

} // namespace velecs::graphics
