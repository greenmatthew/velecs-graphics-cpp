/// @file    TessellationEvaluationShader.cpp
/// @author  Matthew Green
/// @date    2025-07-27 12:05:21
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/Shaders/TessellationEvaluationShader.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

std::shared_ptr<TessellationEvaluationShader> TessellationEvaluationShader::FromCode(
    VkDevice device,
    const std::vector<uint32_t>& spirvCode,
    const std::string& entryPoint/* = "main"*/
)
{
    return std::make_shared<TessellationEvaluationShader>(device, entryPoint, std::filesystem::path{}, spirvCode, ConstructorKey{});
}

std::shared_ptr<TessellationEvaluationShader> TessellationEvaluationShader::FromFile(
    VkDevice device,
    const std::filesystem::path& relPath,
    const std::string& entryPoint/* = "main"*/
)
{
    return std::make_shared<TessellationEvaluationShader>(device, entryPoint, relPath, std::vector<uint32_t>{}, ConstructorKey{});
}

// Public Methods

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::graphics
