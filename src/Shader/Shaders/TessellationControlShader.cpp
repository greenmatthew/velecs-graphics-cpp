/// @file    TessellationControlShader.cpp
/// @author  Matthew Green
/// @date    2025-07-27 12:04:43
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/Shaders/TessellationControlShader.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

std::shared_ptr<TessellationControlShader> TessellationControlShader::FromCode(
    VkDevice device,
    const std::vector<uint32_t>& spirvCode,
    const std::string& entryPoint/* = "main"*/
)
{
    return std::make_shared<TessellationControlShader>(device, entryPoint, std::filesystem::path{}, spirvCode, ConstructorKey{});
}

std::shared_ptr<TessellationControlShader> TessellationControlShader::FromFile(
    VkDevice device,
    const std::filesystem::path& relPath,
    const std::string& entryPoint/* = "main"*/
)
{
    return std::make_shared<TessellationControlShader>(device, entryPoint, relPath, std::vector<uint32_t>{}, ConstructorKey{});
}

// Public Methods

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::graphics
