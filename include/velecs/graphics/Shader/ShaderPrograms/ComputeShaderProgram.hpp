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
#include "velecs/graphics/Shader/Reflection/ShaderReflector.hpp"
#include "velecs/graphics/Shader/Reflection/ShaderReflectionData.hpp"
#include "velecs/graphics/Shader/PushConstant.hpp"
#include "velecs/graphics/ComputePipelineBuilder.hpp"

#include <memory>
#include <optional>

namespace velecs::graphics {

/// @class ComputeShaderProgram
/// @brief Brief description.
///
/// Rest of description.
class ComputeShaderProgram : public ShaderProgramBase {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    ComputeShaderProgram() = default;

    /// @brief Default deconstructor.
    inline ~ComputeShaderProgram() override { Cleanup(); }

    // Public Methods

    /// @brief Checks structural completeness of the computer program
    /// @details Validates that compute shader is assigned
    /// @return True if the program structure meets computer pipeline requirements
    bool IsComplete() const override;

    /// @brief Counts the number of active (non-null) shader stages
    /// @return Number of shader stages that are currently assigned
    size_t GetStageCount() const override;

    void SetComputeShader(const std::shared_ptr<ComputeShader>& shader);

    void SetDescriptor(const VkDescriptorSetLayout descriptorSetLayout, const VkDescriptorSet descriptorSet);

    /// @brief Configures push constants for this compute program (call before Init())
    template<typename PushConstantType>
    void ConfigurePushConstants()
    {
        if (_initialized)
            throw std::runtime_error("Cannot configure push constants after Init() has been called");
        
        if (_comp == nullptr)
            throw std::runtime_error("Cannot configure push constants without a compute shader assigned");

        ShaderReflectionData reflectionData = Reflect(*_comp.get());
        _pushConstant = PushConstant::Create<PushConstantType>(VK_SHADER_STAGE_COMPUTE_BIT, reflectionData);
    }

    void Init(const VkDevice device);

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

    void SetGroupCount(const uint32_t x, const uint32_t y = 1, const uint32_t z = 1);

    void Dispatch(const VkCommandBuffer cmd);

protected:
    // Protected Fields

    // Protected Methods

    /// @brief Validates that all assigned shaders compiled successfully
    /// @details Checks IsValid() on each non-null shader pointer.
    /// @return True if all assigned shaders are valid and ready for use
    bool ValidateShaders() const override;

    void InitPipelineLayout();
    void InitPipeline();

private:
    // Private Fields

    bool _initialized{false};

    VkDevice _device{VK_NULL_HANDLE};

    std::shared_ptr<ComputeShader> _comp;

    VkDescriptorSetLayout _descriptorSetLayout{VK_NULL_HANDLE};
    VkDescriptorSet _descriptorSet{VK_NULL_HANDLE};

    std::optional<PushConstant> _pushConstant;

    std::optional<uint32_t> _numGroupsX{std::nullopt};
    std::optional<uint32_t> _numGroupsY{std::nullopt};
    std::optional<uint32_t> _numGroupsZ{std::nullopt};

    VkPipelineLayout _pipelineLayout{VK_NULL_HANDLE};
    VkPipeline _pipeline{VK_NULL_HANDLE};

    // Private Methods

    void Cleanup()
    {
        if (_comp)
            _comp.reset();

        if (_device)
        {
            if (_pipelineLayout)
                vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
            if (_pipeline)
                vkDestroyPipeline(_device, _pipeline, nullptr);
        }
    }
};

} // namespace velecs::graphics
