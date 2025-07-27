/// @file    ComputeShaderProgram.hpp
/// @author  Matthew Green
/// @date    2025-07-25 14:55:29
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Shader/ShaderPrograms/ShaderProgramBase.hpp"
#include "velecs/graphics/Shader/Shaders/ComputeShader.hpp"

#include <memory>

namespace velecs::graphics {

/// @class ComputeShaderProgram
/// @brief Brief description.
///
/// Rest of description.
class ComputeShaderProgram : public ShaderProgramBase {
public:
    // Enums

    // Public Fields

    std::shared_ptr<ComputeShader> comp;

    // Constructors and Destructors

    /// @brief Default constructor.
    ComputeShaderProgram() = default;

    /// @brief Default deconstructor.
    ~ComputeShaderProgram() = default;

    // Public Methods

    /// @brief Checks structural completeness of the computer program
    /// @details Validates that compute shader is assigned
    /// @return True if the program structure meets computer pipeline requirements
    bool IsComplete() const override;

    /// @brief Counts the number of active (non-null) shader stages
    /// @return Number of shader stages that are currently assigned
    size_t GetStageCount() const override;

protected:
    // Protected Fields

    // Protected Methods

    /// @brief Validates that all assigned shaders compiled successfully
    /// @details Checks IsValid() on each non-null shader pointer.
    /// @return True if all assigned shaders are valid and ready for use
    bool ValidateShaders() const override;

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
