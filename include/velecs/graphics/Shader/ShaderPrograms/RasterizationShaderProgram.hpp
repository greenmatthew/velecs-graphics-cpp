/// @file    RasterizationShaderProgram.hpp
/// @author  Matthew Green
/// @date    2025-07-14 13:20:43
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Shader/ShaderPrograms/ShaderProgramBase.hpp"
#include "velecs/graphics/Shader/Shaders/VertexShader.hpp"
#include "velecs/graphics/Shader/Shaders/GeometryShader.hpp"
#include "velecs/graphics/Shader/Shaders/FragmentShader.hpp"
#include "velecs/graphics/Shader/Shaders/TessellationControlShader.hpp"
#include "velecs/graphics/Shader/Shaders/TessellationEvaluationShader.hpp"

namespace velecs::graphics {

/// @struct RasterizationShaderProgram
/// @brief Brief description.
///
/// Rest of description.
struct RasterizationShaderProgram : public ShaderProgramBase {
public:
    // Enums

    // Public Fields

    std::shared_ptr<VertexShader> vert{nullptr}; /// @brief Vertex shader (required)
    std::shared_ptr<FragmentShader> frag{nullptr}; /// @brief Fragment shader (required)
    std::shared_ptr<GeometryShader> geom{nullptr}; /// @brief Geometry shader (optional)
    std::shared_ptr<TessellationControlShader> tesc{nullptr}; /// @brief Tessellation control shader (optional - must pair with tese)
    std::shared_ptr<TessellationEvaluationShader> tese{nullptr}; /// @brief Tessellation evaluation shader (optional - must pair with tesc)

    // Constructors and Destructors

    /// @brief Default constructor.
    RasterizationShaderProgram() = default;

    /// @brief Default deconstructor.
    ~RasterizationShaderProgram() = default;

    // Public Methods

    /// @brief Checks structural completeness of the rasterization program
    /// @details Validates that:
    ///          - Vertex and fragment shaders are assigned (required)
    ///          - Tessellation shaders are properly paired (both or neither)
    /// @return True if the program structure meets rasterization pipeline requirements
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
