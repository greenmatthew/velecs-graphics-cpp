/// @file    Shader.hpp
/// @author  Matthew Green
/// @date    2025-07-13 13:51:31
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/VulkanInitializers.hpp"

#include <vulkan/vulkan_core.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <filesystem>

namespace velecs::graphics {

/// @class Shader
/// @brief Encapsulates a Vulkan shader module with metadata for pipeline creation.
///
/// This class manages the lifecycle of a Vulkan shader module and provides
/// convenient methods for loading from files and integrating with pipelines.
/// File paths are interpreted as relative to the assets directory.
class Shader {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Constructor access key to enforce factory method usage
    class ConstructorKey {
        friend class Shader;
        ConstructorKey() = default;
    };

    /// @brief Constructor for creating a shader from file (use factory methods instead)
    /// @param device The Vulkan device handle
    /// @param stage The shader stage type
    /// @param relPath Path to the SPIR-V file relative to the assets directory
    /// @param entryPoint The entry point function name
    /// @param key Access key (only accessible via factory methods)
    inline Shader(
        VkDevice device,
        VkShaderStageFlagBits stage,
        const std::filesystem::path& relPath,
        const std::string& entryPoint,
        ConstructorKey key
    ) : _device(device), _stage(stage), _relPath(relPath), _entryPoint(entryPoint) 
    {
        BuildFromFile();
    }

    /// @brief Constructor for creating a shader from code (use factory methods instead)
    /// @param device The Vulkan device handle
    /// @param stage The shader stage type
    /// @param spirvCode The compiled SPIR-V bytecode
    /// @param entryPoint The entry point function name
    /// @param key Access key (only accessible via factory methods)
    inline Shader(
        VkDevice device,
        VkShaderStageFlagBits stage,
        const std::vector<uint32_t>& spirvCode,
        const std::string& entryPoint,
        ConstructorKey key
    ) : _device(device), _stage(stage), _spirvCode(spirvCode), _entryPoint(entryPoint)
    {
        BuildFromCode();
    }

    /// @brief Destructor - cleans up Vulkan resources
    ~Shader() { Cleanup(); }

    // Delete copy operations to prevent accidental copying of Vulkan handles
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Allow move operations
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    // Public Methods

    /// @brief Creates a shader from SPIR-V bytecode
    /// @param device The Vulkan device handle
    /// @param stage The shader stage type
    /// @param spirvCode The compiled SPIR-V bytecode
    /// @param entryPoint The entry point function name (default: "main")
    /// @return Unique pointer to the created shader
    static std::unique_ptr<Shader> FromCode(
        VkDevice device,
        VkShaderStageFlagBits stage,
        const std::vector<uint32_t>& spirvCode,
        const std::string& entryPoint = "main"
    );

    /// @brief Creates a shader from a SPIR-V file in the assets directory
    /// @param device The Vulkan device handle
    /// @param stage The shader stage type
    /// @param relPath Path to the SPIR-V file relative to the assets directory (e.g., "shaders/vertex.spv")
    /// @param entryPoint The entry point function name (default: "main")
    /// @return Unique pointer to the created shader
    /// @note The file path is resolved relative to Paths::AssetsDir()
    static std::unique_ptr<Shader> FromFile(
        VkDevice device,
        VkShaderStageFlagBits stage,
        const std::filesystem::path& relPath,
        const std::string& entryPoint = "main"
    );

    /// @brief Checks if the shader module is valid
    /// @return True if the shader module was successfully created
    inline bool IsValid() const { return _module != VK_NULL_HANDLE; }

    /// @brief Gets the Vulkan device handle
    /// @return The device handle
    inline VkDevice GetDevice() const { return _device; }

    /// @brief Gets the shader stage
    /// @return The shader stage flags
    inline VkShaderStageFlagBits GetStage() const { return _stage; }

    /// @brief Gets the relative file path
    /// @return The file path relative to the assets directory (empty if created from code)
    inline const std::filesystem::path& GetFilePath() const { return _relPath; }

    /// @brief Gets the SPIR-V bytecode
    /// @return Reference to the SPIR-V code
    const std::vector<uint32_t>& GetSpirVCode() const { return _spirvCode; }

    /// @brief Gets the entry point function name
    /// @return The entry point name
    inline const std::string& GetEntryPoint() const { return _entryPoint; }

    /// @brief Gets the Vulkan shader module handle
    /// @return The shader module handle
    inline VkShaderModule GetShaderModule() const { return _module; }

    /// @brief Gets the pipeline shader stage create info
    /// @return Create info for use with VkGraphicsPipelineCreateInfo
    inline VkPipelineShaderStageCreateInfo GetCreateInfo() const { return _stageCreateInfo; }

    /// @brief Reloads the shader from its source file (only works for file-based shaders)
    /// @return Reference to this shader for method chaining
    /// @throws std::runtime_error if no file path or reload fails
    Shader& Reload();

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    VkDevice _device{VK_NULL_HANDLE};                    /// @brief The Vulkan device handle
    VkShaderStageFlagBits _stage{};                      /// @brief The shader stage type
    std::filesystem::path _relPath;                      /// @brief File path relative to `Paths::AssetsDir()`
    std::string _entryPoint;                             /// @brief Entry point function name
    std::vector<uint32_t> _spirvCode;                    /// @brief SPIR-V bytecode
    VkShaderModule _module{VK_NULL_HANDLE};              /// @brief The compiled shader module
    VkPipelineShaderStageCreateInfo _stageCreateInfo{};  /// @brief Pipeline stage create info

    // Private Methods

    /// @brief Builds shader module from stored code (called during construction)
    void BuildFromCode();

    /// @brief Builds shader module from file (called during construction)
    void BuildFromFile();

    /// @brief Creates a Vulkan shader module from SPIR-V bytecode
    /// @param spirvCode The SPIR-V bytecode
    /// @return The created shader module handle
    /// @throws std::runtime_error on creation failure
    VkShaderModule CreateModuleFromCode(const std::vector<uint32_t>& spirvCode);

    /// @brief Loads SPIR-V bytecode from a file relative to the assets directory
    /// @param relPath Path to the SPIR-V file relative to the assets directory
    /// @return Vector containing the bytecode
    /// @throws std::runtime_error on file reading failure
    static std::vector<uint32_t> LoadSpirVFromFile(const std::filesystem::path& relPath);

    /// @brief Cleans up Vulkan resources
    void Cleanup();
};

} // namespace velecs::graphics
