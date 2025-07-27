/// @file    TessellationEvaluationShader.hpp
/// @author  Matthew Green
/// @date    2025-07-27 11:22:55
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Shader/Shaders/Shader.hpp"

namespace velecs::graphics {

/// @class TessellationEvaluationShader
/// @brief Brief description.
///
/// Rest of description.
class TessellationEvaluationShader : public Shader {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    struct ConstructorKey {
        friend class TessellationEvaluationShader;
        ConstructorKey() = default;
    };

    /// @brief Constructor for creating a shader from file (use factory methods instead)
    /// @param device The Vulkan device handle
    /// @param entryPoint The entry point function name
    /// @param relPath Path to the SPIR-V file relative to assets directory (empty for code-based)
    /// @param spirvCode The compiled SPIR-V bytecode (empty for file-based)
    inline TessellationEvaluationShader(
        VkDevice device,
        const std::string& entryPoint,
        const std::filesystem::path& relPath,
        const std::vector<uint32_t>& spirvCode,
        ConstructorKey
    ) : Shader(device, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, entryPoint, relPath, spirvCode) {}

    /// @brief Default constructor.
    TessellationEvaluationShader() = delete;

    /// @brief Default deconstructor.
    virtual ~TessellationEvaluationShader() = default;

    /// @brief Creates a tessellation evaluation shader from SPIR-V bytecode
    /// @param device The Vulkan device handle
    /// @param spirvCode The compiled SPIR-V bytecode
    /// @param entryPoint The entry point function name (default: "main")
    /// @return Shared pointer to the created tessellation evaluation shader
    static std::shared_ptr<TessellationEvaluationShader> FromCode(
        VkDevice device,
        const std::vector<uint32_t>& spirvCode,
        const std::string& entryPoint = "main"
    );

    /// @brief Creates a tessellation evaluation shader from a SPIR-V file in the assets directory
    /// @param device The Vulkan device handle
    /// @param relPath Path to the SPIR-V file relative to the assets directory (e.g., "shaders/vertex.spv")
    /// @param entryPoint The entry point function name (default: "main")
    /// @return Shared pointer to the created tessellation evaluation shader
    /// @note The file path is resolved relative to Paths::AssetsDir()
    static std::shared_ptr<TessellationEvaluationShader> FromFile(
        VkDevice device,
        const std::filesystem::path& relPath,
        const std::string& entryPoint = "main"
    );

    // Public Methods

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
