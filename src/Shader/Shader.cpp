// @file    Shader.cpp
/// @author  Matthew Green
/// @date    2025-07-13 14:12:43
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/Shader.hpp"

#include <velecs/common/Paths.hpp>
using namespace velecs::common;

#include <iostream>
#include <sstream>

namespace velecs::graphics {

// Constructors and Destructors

Shader::Shader(Shader&& other) noexcept
    : _device(other._device),
        _stage(other._stage),
        _relPath(std::move(other._relPath)),
        _entryPoint(std::move(other._entryPoint)),
        _spirvCode(std::move(other._spirvCode)),
        _module(other._module),
        _stageCreateInfo(other._stageCreateInfo)
{
    other._device = VK_NULL_HANDLE;
    other._module = VK_NULL_HANDLE;
    other._stageCreateInfo = {};
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        Cleanup();

        _device = other._device;
        _stage = other._stage;
        _relPath = std::move(other._relPath);
        _entryPoint = std::move(other._entryPoint);
        _spirvCode = std::move(other._spirvCode);
        _module = other._module;
        _stageCreateInfo = other._stageCreateInfo;

        other._device = VK_NULL_HANDLE;
        other._module = VK_NULL_HANDLE;
        other._stageCreateInfo = {};
    }
    return *this;
}

// Public Methods

std::unique_ptr<Shader> Shader::FromFile(
    VkDevice device,
    VkShaderStageFlagBits stage,
    const std::filesystem::path& relPath,
    const std::string& entryPoint
)
{
    return std::make_unique<Shader>(device, stage, relPath, entryPoint, ConstructorKey{});
}

std::unique_ptr<Shader> Shader::FromCode(
    VkDevice device,
    VkShaderStageFlagBits stage,
    const std::vector<uint32_t>& spirvCode,
    const std::string& entryPoint
)
{
    return std::make_unique<Shader>(device, stage, spirvCode, entryPoint, ConstructorKey{});
}

Shader& Shader::Reload()
{
    // Clean up existing module first
    Cleanup();

    if (_relPath.empty())
    {
        // Rebuild from stored SPIR-V code
        BuildFromCode();
    }
    else
    {
        // Rebuild from file (re-reads from disk)
        BuildFromFile();
    }

    return *this;
}

// Private Methods

void Shader::BuildFromFile()
{
    if (_relPath.empty())
    {
        throw std::runtime_error("Cannot build shader from file: no file path provided");
    }

    std::vector<uint32_t> spirvCode = LoadSpirVFromFile(_relPath);
    _module = CreateModuleFromCode(spirvCode);
    _stageCreateInfo = VkExtPipelineShaderStageCreateInfo(_stage, _module);
}

void Shader::BuildFromCode()
{
    if (_spirvCode.empty())
    {
        throw std::runtime_error("Cannot build shader from code: no SPIR-V code provided");
    }

    _module = CreateModuleFromCode(_spirvCode);
    _stageCreateInfo = VkExtPipelineShaderStageCreateInfo(_stage, _module);
}

VkShaderModule Shader::CreateModuleFromCode(const std::vector<uint32_t>& spirvCode)
{
    if (spirvCode.empty())
    {
        throw std::runtime_error("Cannot create shader module from empty SPIR-V code");
    }

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;

    // codeSize has to be in bytes, so multiply the ints in the buffer by size of int to know the real size of the buffer
    createInfo.codeSize = spirvCode.size() * sizeof(uint32_t);
    createInfo.pCode = spirvCode.data();

    // check that the creation goes well.
    VkShaderModule shaderModule;
    VkResult result = vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS)
    {
        std::ostringstream oss;
        oss << "Failed to create shader module: " << result;
        throw std::runtime_error(oss.str());
    }

    return shaderModule;
}

std::vector<uint32_t> Shader::LoadSpirVFromFile(const std::filesystem::path& relPath)
{
    auto filePath = Paths::AssetsDir() / relPath;

    // open the file. With cursor at the end
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open shader file: " + filePath.string());
    }

    // find what the size of the file is by looking up the location of the cursor
    // because the cursor is at the end, it gives the size directly in bytes
    size_t fileSize = static_cast<size_t>(file.tellg());

    if (fileSize == 0)
    {
        throw std::runtime_error("Shader file is empty: " + filePath.string());
    }

    // SPIR-V files should be aligned to 4-byte boundaries
    if (fileSize % sizeof(uint32_t) != 0)
    {
        throw std::runtime_error("Invalid SPIR-V file size (not aligned to 4 bytes): " + filePath.string());
    }

    // spirv expects the buffer to be on uint32, so make sure to reserve an int vector big enough for the entire file
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    // put file cursor at beginning
    file.seekg(0);

    // load the entire file into the buffer
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    // now that the file is loaded into the buffer, we can close it
    file.close();

    // Basic SPIR-V magic number validation
    if (!buffer.empty() && buffer[0] != 0x07230203)
    {
        throw std::runtime_error("Invalid SPIR-V magic number in file: " + filePath.string());
    }

    return buffer;
}

void Shader::Cleanup()
{
    if (_module != VK_NULL_HANDLE && _device != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(_device, _module, nullptr);
        _module = VK_NULL_HANDLE;
    }
    _stageCreateInfo = {};
}

} // namespace velecs::graphics
