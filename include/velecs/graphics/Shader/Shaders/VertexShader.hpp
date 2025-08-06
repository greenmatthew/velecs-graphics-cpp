/// @file    VertexShader.hpp
/// @author  Matthew Green
/// @date    2025-07-26 13:19:01
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Shader/Shaders/Shader.hpp"

namespace velecs::graphics {

/// @class VertexShader
/// @brief Brief description.
///
/// Rest of description.
class VertexShader : public Shader {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    struct ConstructorKey {
        friend class VertexShader;
        ConstructorKey() = default;
    };

    /// @brief Constructor for creating a shader from file (use factory methods instead)
    /// @param entryPoint The entry point function name
    /// @param relPath Path to the SPIR-V file relative to assets directory (empty for code-based)
    /// @param spirvCode The compiled SPIR-V bytecode (empty for file-based)
    inline VertexShader(
        const std::string& entryPoint,
        const std::filesystem::path& relPath,
        const std::vector<uint32_t>& spirvCode,
        ConstructorKey
    ) : Shader(VK_SHADER_STAGE_VERTEX_BIT, entryPoint, relPath, spirvCode) {}

    /// @brief Default constructor.
    VertexShader() = delete;

    /// @brief Default deconstructor.
    virtual ~VertexShader() = default;

    /// @brief Creates a vertex shader from SPIR-V bytecode
    /// @param spirvCode The compiled SPIR-V bytecode
    /// @param entryPoint The entry point function name (default: "main")
    /// @return Shared pointer to the created vertex shader
    static std::shared_ptr<VertexShader> FromCode(
        const std::vector<uint32_t>& spirvCode,
        const std::string& entryPoint = "main"
    );

    /// @brief Creates a vertex shader from a SPIR-V file in the assets directory
    /// @param relPath Path to the SPIR-V file relative to the assets directory (e.g., "shaders/vertex.spv")
    /// @param entryPoint The entry point function name (default: "main")
    /// @return Shared pointer to the created vertex shader
    /// @note The file path is resolved relative to Paths::AssetsDir()
    static std::shared_ptr<VertexShader> FromFile(
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
