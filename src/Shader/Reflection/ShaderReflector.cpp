/// @file    ShaderReflector.cpp
/// @author  Matthew Green
/// @date    2025-07-14 14:16:22
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/Reflection/ShaderReflector.hpp"

#include "velecs/graphics/Shader/Shaders/Shader.hpp"
#include "velecs/graphics/Shader/Reflection/ShaderResource.hpp"

#include <spirv_cross/spirv_cross.hpp>

#include <exception>
#include <vector>

namespace velecs::graphics {

namespace {  // Anonymous namespace for private implementation

ShaderReflectionData ParseSpirV(const std::vector<uint32_t>& spirvCode, VkShaderStageFlagBits stage)
{
    ShaderReflectionData data;
    
    // Create the compiler
    spirv_cross::Compiler compiler(spirvCode);
    
    // Get all shader resources in one call!
    spirv_cross::ShaderResources spirvResources = compiler.get_shader_resources();
    
    // Extract uniform buffers (UBOs)
    for (const auto& ubo : spirvResources.uniform_buffers) {
        ShaderResource resource;

        resource.type = ShaderResourceType::UniformBuffer;
        resource.stages |= stage;

        resource.name = ubo.name;
        resource.set = compiler.get_decoration(ubo.id, spv::DecorationDescriptorSet);
        resource.binding = compiler.get_decoration(ubo.id, spv::DecorationBinding);
        const auto& type = compiler.get_type(ubo.base_type_id);
        resource.size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));
        
        data.uniformBuffers.push_back(resource);
    }
    
    // Extract samplers/textures
    for (const auto& sampler : spirvResources.sampled_images) {
        ShaderResource resource;

        resource.type = ShaderResourceType::Texture2D;
        resource.stages |= stage;

        resource.name = sampler.name;
        resource.set = compiler.get_decoration(sampler.id, spv::DecorationDescriptorSet);
        resource.binding = compiler.get_decoration(sampler.id, spv::DecorationBinding);
        resource.size = 0; // Samplers don't have a size
        
        data.textures.push_back(resource);
    }
    
    // Extract push constants
    for (const auto& pushConstant : spirvResources.push_constant_buffers)
    {
        ShaderResource resource;

        resource.type = ShaderResourceType::PushConstant;
        resource.stages |= stage;

        resource.name = pushConstant.name;
        const auto type = compiler.get_type(pushConstant.base_type_id);
        resource.size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));

        data.pushConstants.push_back(resource);
    }
    
    return data;
}

} // anonymous namespace

// Public interface implementation
ShaderReflectionData Reflect(const Shader& shader)
{
    if (!shader.IsValid()) throw std::runtime_error("Cannot reflect invalid shader");

    const auto& spirvCode = shader.GetSpirVCode();
    return ParseSpirV(spirvCode, shader.GetStage());
}

} // namespace velecs::graphics