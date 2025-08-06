/// @file    PipelineBuilderBase.hpp
/// @author  Matthew Green
/// @date    2025-07-25 15:11:14
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <vulkan/vulkan_core.h>

namespace velecs::graphics {

/// @class PipelineBuilderBase
/// @brief Abstract base class for Vulkan pipeline builders using the builder pattern.
///
/// Provides common functionality for building different types of Vulkan pipelines
/// (graphics, compute, ray tracing) with a fluent interface. Derived classes implement
/// specific pipeline creation logic while sharing common setup and validation.
template<typename Derived>
class PipelineBuilderBase {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor
    PipelineBuilderBase() = default;

    /// @brief Pure virtual destructor to ensure proper cleanup of derived classes.
    virtual ~PipelineBuilderBase() = 0;

    // Delete copy operations
    PipelineBuilderBase(const PipelineBuilderBase&) = delete;
    PipelineBuilderBase& operator=(const PipelineBuilderBase&) = delete;

    // Allow move operations  
    PipelineBuilderBase(PipelineBuilderBase&& other) noexcept = default;
    PipelineBuilderBase& operator=(PipelineBuilderBase&& other) noexcept = default;

    // Public Methods

    /// @brief Sets the Vulkan device handle for pipeline creation.
    /// @param device Valid Vulkan device handle
    /// @return Reference to this builder for method chaining
    Derived& SetDevice(const VkDevice device)
    {
        _device = device;
        return derived();
    }

    /// @brief Sets the pipeline layout that describes resource bindings.
    /// @param pipelineLayout Valid Vulkan pipeline layout handle
    /// @return Reference to this builder for method chaining
    Derived& SetPipelineLayout(const VkPipelineLayout pipelineLayout)
    {
        _pipelineLayout = pipelineLayout;
        return derived();
    }

    /// @brief Creates and returns the configured Vulkan pipeline.
    /// @return Handle to the created Vulkan pipeline
    /// @throws std::runtime_error if validation fails or pipeline creation fails
    /// @details Validates the current state, then delegates to derived class implementation.
    ///          The pipeline must be manually destroyed when no longer needed.
    VkPipeline GetPipeline()
    {
        return CreatePipeline();
    }

protected:
    // Protected Fields

    VkDevice _device{VK_NULL_HANDLE};                 /// @brief Vulkan device handle for pipeline creation
    VkPipelineLayout _pipelineLayout{VK_NULL_HANDLE}; /// @brief Pipeline layout describing resource bindings and push constants

    // Protected Methods

    /// @brief Creates the actual Vulkan pipeline using the configured state.
    /// @return Handle to the created Vulkan pipeline
    /// @throws std::runtime_error if pipeline creation fails
    /// @details Called after successful validation. Override to implement
    ///          type-specific pipeline creation logic. Assumes valid state.
    virtual VkPipeline CreatePipeline() = 0;

private:
    // Private Fields

    // Private Methods

    Derived& derived() { return static_cast<Derived&>(*this); }
};

// Required implementation for pure virtual destructor
template<typename Derived>
PipelineBuilderBase<Derived>::~PipelineBuilderBase() = default;

} // namespace velecs::graphics
