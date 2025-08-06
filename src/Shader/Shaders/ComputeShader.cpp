/// @file    ComputeShader.cpp
/// @author  Matthew Green
/// @date    2025-07-26 13:01:22
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/Shaders/ComputeShader.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

std::shared_ptr<ComputeShader> ComputeShader::FromCode(
    const std::vector<uint32_t>& spirvCode,
    const std::string& entryPoint/* = "main"*/
)
{
    return std::make_shared<ComputeShader>(entryPoint, std::filesystem::path{}, spirvCode, ConstructorKey{});
}

std::shared_ptr<ComputeShader> ComputeShader::FromFile(
    const std::filesystem::path& relPath,
    const std::string& entryPoint/* = "main"*/
)
{
    return std::make_shared<ComputeShader>(entryPoint, relPath, std::vector<uint32_t>{}, ConstructorKey{});
}

// Public Methods

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::graphics
