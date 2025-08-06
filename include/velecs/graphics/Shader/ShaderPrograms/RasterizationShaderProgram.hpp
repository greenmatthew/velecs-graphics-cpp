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
#include "velecs/graphics/RenderPipelineBuilder.hpp"

#include <vulkan/vulkan_core.h>

#include <optional>

namespace velecs::graphics {

/// @struct RasterizationShaderProgram
/// @brief Brief description.
///
/// Rest of description.
struct RasterizationShaderProgram : public ShaderProgramBase {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    RasterizationShaderProgram() = default;

    /// @brief Default deconstructor.
    virtual ~RasterizationShaderProgram() { Cleanup(); }

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

    void SetVertexShader(const std::shared_ptr<VertexShader>& vert);
    void SetGeometryShader(const std::shared_ptr<GeometryShader>& geom);
    void SetFragmentShader(const std::shared_ptr<FragmentShader>& frag);
    void SetTessellationControlShader(const std::shared_ptr<TessellationControlShader>& tesc);
    void SetTessellationEvaluationShader(const std::shared_ptr<TessellationEvaluationShader>& tese);

    void Init(const VkDevice device, const VkPipelineLayout pipelineLayout, RenderPipelineBuilder& pipelineBuilder);
    
    void Draw(const VkCommandBuffer cmd, const VkExtent2D extent);

protected:
    // Protected Fields

    // Protected Methods

    /// @brief Validates that all assigned shaders compiled successfully
    /// @details Checks IsValid() on each non-null shader pointer.
    /// @return True if all assigned shaders are valid and ready for use
    bool ValidateShaders() const override;

    VkShaderStageFlags GetShaderStages() override;
    ShaderReflectionData GetReflectionData() override;

private:
    // Private Fields

    bool _initialized{false};

    std::shared_ptr<VertexShader>                 _vert{nullptr}; /// @brief Vertex shader (required)
    std::shared_ptr<GeometryShader>               _geom{nullptr}; /// @brief Geometry shader (optional)
    std::shared_ptr<FragmentShader>               _frag{nullptr}; /// @brief Fragment shader (required)
    std::shared_ptr<TessellationControlShader>    _tesc{nullptr}; /// @brief Tessellation control shader (optional - must pair with tese)
    std::shared_ptr<TessellationEvaluationShader> _tese{nullptr}; /// @brief Tessellation evaluation shader (optional - must pair with tesc)

    VkPipeline _pipeline{VK_NULL_HANDLE};

    VkDevice _device{VK_NULL_HANDLE};

    // Private Methods

    void Cleanup();
};

} // namespace velecs::graphics
