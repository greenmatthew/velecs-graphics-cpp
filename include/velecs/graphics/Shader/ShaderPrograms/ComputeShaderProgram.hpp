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

    void Init(const VkDevice device);

    void SetGroupCount(const uint32_t x, const uint32_t y = 1, const uint32_t z = 1);

    void Dispatch(const VkCommandBuffer cmd);

protected:
    // Protected Fields

    // Protected Methods

    /// @brief Validates that all assigned shaders compiled successfully
    /// @details Checks IsValid() on each non-null shader pointer.
    /// @return True if all assigned shaders are valid and ready for use
    bool ValidateShaders() const override;

    VkShaderStageFlags GetShaderStages() override;
    ShaderReflectionData GetReflectionData() override;

    void InitPipelineLayout();
    void InitPipeline();

private:
    // Private Fields

    std::shared_ptr<ComputeShader> _comp;

    VkDescriptorSetLayout _descriptorSetLayout{VK_NULL_HANDLE};
    VkDescriptorSet _descriptorSet{VK_NULL_HANDLE};

    std::optional<uint32_t> _numGroupsX{std::nullopt};
    std::optional<uint32_t> _numGroupsY{std::nullopt};
    std::optional<uint32_t> _numGroupsZ{std::nullopt};

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
