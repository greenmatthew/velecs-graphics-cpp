/// @file    ShaderProgramBase.hpp
/// @author  Matthew Green
/// @date    2025-07-14 13:19:49
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Shader/Reflection/ShaderReflectionData.hpp"
#include "velecs/graphics/Shader/PushConstant.hpp"

#include <vulkan/vulkan_core.h>

#include <optional>

namespace velecs::graphics {

/// @struct ShaderProgramBase
/// @brief Brief description.
///
/// Rest of description.
struct ShaderProgramBase {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    ShaderProgramBase() = default;

    /// @brief Virtual destructor for proper cleanup of derived types
    virtual ~ShaderProgramBase() = 0;

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

    /// @brief Configures push constants for this compute program (call before Init())
    template<typename PushConstantType>
    void ConfigurePushConstants()
    {
        if (_initialized)
            throw std::runtime_error("Cannot configure push constants after Init() has been called");

        if (!IsComplete())
            throw std::runtime_error("Cannot configure push constants without shader(s) assigned");

        _pushConstant = PushConstant::Create<PushConstantType>(
            GetShaderStages(),
            GetReflectionData()
        );
    }

    template<typename PushConstantType>
    PushConstantType& GetPushConstant()
    {
        if (!_initialized)
            throw std::runtime_error("Must call Init() before updating push constants");

        if (!_pushConstant)
            throw std::runtime_error("There is no push constant configured to update");
        
        return _pushConstant->GetData<PushConstantType>();
    }

    template<typename PushConstantType>
    const PushConstantType& GetPushConstant() const
    {
        if (!_initialized)
            throw std::runtime_error("Must call Init() before updating push constants");

        if (!_pushConstant)
            throw std::runtime_error("There is no push constant configured to update");
        
        return _pushConstant->GetData<PushConstantType>();
    }

    /// @brief Updates push constant data (fast runtime call)
    template<typename PushConstantType>
    void UpdatePushConstant(const PushConstantType& data)
    {
        if (!_initialized)
            throw std::runtime_error("Must call Init() before updating push constants");

        if (!_pushConstant)
            throw std::runtime_error("There is no push constant configured to update");
        
        _pushConstant->UpdateData(data);
    }

protected:
    // Protected Fields

    bool _initialized{false};

    std::optional<PushConstant> _pushConstant;

    VkPipelineLayout _pipelineLayout{VK_NULL_HANDLE};
    VkPipeline _pipeline{VK_NULL_HANDLE};

    VkDevice _device;

    // Protected Methods

    /// @brief Validates that all assigned shaders compiled successfully
    /// @details This method should check each non-null shader's IsValid() status.
    ///          It's called automatically by IsValid() after IsComplete() passes.
    /// @return True if all assigned shaders are valid
    virtual bool ValidateShaders() const = 0;

    virtual VkShaderStageFlags GetShaderStages() = 0;
    virtual ShaderReflectionData GetReflectionData() = 0;

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
