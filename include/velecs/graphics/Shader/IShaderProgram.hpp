/// @file    IShaderProgram.hpp
/// @author  Matthew Green
/// @date    2025-07-14 13:19:49
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

namespace velecs::graphics {

/// @struct IShaderProgram
/// @brief Brief description.
///
/// Rest of description.
struct IShaderProgram {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    IShaderProgram() = default;

    /// @brief Virtual destructor for proper cleanup of derived types
    virtual ~IShaderProgram() = default;

    // Public Methods

    /// @brief Checks if the shader program has all required stages assigned
    /// @details This validates the structural completeness of the program without
    ///          checking if individual shaders are valid. For example, it ensures
    ///          required shaders are present and optional dependencies are satisfied.
    /// @return True if all required shader stages are assigned (non-null)
    virtual bool IsComplete() const = 0;

    /// @brief Comprehensive validation check for pipeline creation readiness
    /// @details Performs both structural validation (IsComplete) and content validation
    ///          (ValidateShaders) to ensure the program is ready for use.
    /// @return True if the program is complete and all shaders are valid
    inline bool IsValid() const
    {
        if (!IsComplete()) return false;
        return ValidateShaders();
    }

    /// @brief Gets the number of active shader stages in this program
    /// @return Count of non-null shader stages
    virtual size_t GetStageCount() const = 0;

protected:
    // Protected Fields

    // Protected Methods

    /// @brief Validates that all assigned shaders compiled successfully
    /// @details This method should check each non-null shader's IsValid() status.
    ///          It's called automatically by IsValid() after IsComplete() passes.
    /// @return True if all assigned shaders are valid
    virtual bool ValidateShaders() const = 0;

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
