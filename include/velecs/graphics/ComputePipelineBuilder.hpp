/// @file    ComputePipelineBuilder.hpp
/// @author  Matthew Green
/// @date    2025-07-25 15:09:13
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/PipelineBuilderBase.hpp"
#include "velecs/graphics/Shader/Shaders/ComputeShader.hpp"

namespace velecs::graphics {

/// @class ComputePipelineBuilder
/// @brief Brief description.
///
/// Rest of description.
class ComputePipelineBuilder : public PipelineBuilderBase<ComputePipelineBuilder> {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    ComputePipelineBuilder() = default;

    /// @brief Pure virtual destructor to ensure proper cleanup of derived classes.
    virtual ~ComputePipelineBuilder() = default;

    // Public Methods

    ComputePipelineBuilder& SetComputeShader(const std::shared_ptr<ComputeShader>& compShader);

protected:
    // Protected Fields

    std::shared_ptr<Shader> _compShader; /// @brief Compute shader to be used in the pipeline

    // Protected Methods

    /// @brief Validates that all required pipeline state has been configured.
    /// @throws std::runtime_error if any required state is missing or invalid
    /// @details Called before pipeline creation to ensure all necessary
    ///          parameters have been set. Override to add type-specific validation.
    virtual void ValidateState() override;

    /// @brief Creates the actual Vulkan pipeline using the configured state.
    /// @return Handle to the created Vulkan pipeline
    /// @throws std::runtime_error if pipeline creation fails
    /// @details Called after successful validation. Override to implement
    ///          type-specific pipeline creation logic. Assumes valid state.
    virtual VkPipeline CreatePipeline() override;

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
