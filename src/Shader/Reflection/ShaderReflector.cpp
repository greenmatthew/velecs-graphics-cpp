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

ShaderMemberType MapSpirVTypeToShaderMemberType(const spirv_cross::SPIRType& spirvType)
{
    switch (spirvType.basetype)
    {
    case spirv_cross::SPIRType::Float:
        switch (spirvType.vecsize)
        {
            case 1: return ShaderMemberType::Float;
            case 2: return ShaderMemberType::Vec2;
            case 3: return ShaderMemberType::Vec3;
            case 4: return ShaderMemberType::Vec4;
        }
        if (spirvType.columns > 1)
        {
            switch (spirvType.columns)
            {
                // case 2: return ShaderMemberType::Mat2;
                // case 3: return ShaderMemberType::Mat3;
                case 4: return ShaderMemberType::Mat4;
            }
        }
        break;
        
    case spirv_cross::SPIRType::Int:
        switch (spirvType.vecsize) {
            case 1: return ShaderMemberType::Int;
            // case 2: return ShaderMemberType::IVec2;
            // case 3: return ShaderMemberType::IVec3;
            // case 4: return ShaderMemberType::IVec4;
        }
        break;
        
    case spirv_cross::SPIRType::UInt:
        switch (spirvType.vecsize) {
            case 1: return ShaderMemberType::UInt;
            // case 2: return ShaderMemberType::UVec2;
            // case 3: return ShaderMemberType::UVec3;
            // case 4: return ShaderMemberType::UVec4;
        }
        break;
        
    case spirv_cross::SPIRType::Struct: return ShaderMemberType::Struct;
    }

    // Default fallback
    return ShaderMemberType::Unknown;
}

std::vector<ShaderMember> ExtractStructMembers(const spirv_cross::Compiler& compiler, const spirv_cross::SPIRType& structType)
{
    std::vector<ShaderMember> members;
    
    for (uint32_t i{0}; i < structType.member_types.size(); ++i)
    {
        ShaderMember member;
        member.name = compiler.get_member_name(structType.self, i);
        const auto& memberType = compiler.get_type(structType.member_types[i]);
        member.offset = compiler.type_struct_member_offset(structType, i);
        member.size = static_cast<uint32_t>(compiler.get_declared_struct_member_size(structType, i));
        member.arraySize = memberType.array.empty() ? 0 : memberType.array[0];
        member.type = MapSpirVTypeToShaderMemberType(memberType);
        
        // If it's a struct, recursively extract its members
        if (memberType.basetype == spirv_cross::SPIRType::Struct)
        {
            member.members = ExtractStructMembers(compiler, memberType);
        }
        
        members.push_back(member);
    }
    
    return members;
}

ShaderReflectionData ParseSpirV(const std::vector<uint32_t>& spirvCode, VkShaderStageFlagBits stage)
{
    ShaderReflectionData data;
    spirv_cross::Compiler compiler(spirvCode);
    spirv_cross::ShaderResources spirvResources = compiler.get_shader_resources();
    
    // Extract uniform buffers (UBOs)
    for (const auto& ubo : spirvResources.uniform_buffers)
    {
        ShaderResource resource;
        resource.type = ShaderResourceType::UniformBuffer;
        resource.stages |= stage;
        resource.name = ubo.name;
        resource.set = compiler.get_decoration(ubo.id, spv::DecorationDescriptorSet);
        resource.binding = compiler.get_decoration(ubo.id, spv::DecorationBinding);
        const auto& type = compiler.get_type(ubo.base_type_id);
        resource.size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));
        resource.members = ExtractStructMembers(compiler, type);
        
        data.uniformBuffers.push_back(resource);
    }

    // Extract storage images
    for (const auto& storageImage : spirvResources.storage_images)
    {
        ShaderResource resource;
        resource.type = ShaderResourceType::StorageImage; // You'll need to add this enum value
        resource.stages |= stage;
        resource.name = storageImage.name;
        resource.set = compiler.get_decoration(storageImage.id, spv::DecorationDescriptorSet);
        resource.binding = compiler.get_decoration(storageImage.id, spv::DecorationBinding);
        resource.size = 0; // Storage images don't have a traditional size
        
        data.storageImages.push_back(resource); // You'll need to add this vector to ShaderReflectionData
    }
    
    // Extract samplers/textures
    for (const auto& sampler : spirvResources.sampled_images)
    {
        std::cout << "TESTING!" << std::endl;
        ShaderResource resource;
        resource.type = ShaderResourceType::SampledImage;
        resource.stages |= stage;
        resource.name = sampler.name;
        resource.set = compiler.get_decoration(sampler.id, spv::DecorationDescriptorSet);
        resource.binding = compiler.get_decoration(sampler.id, spv::DecorationBinding);
        resource.size = 0; // Samplers don't have a size
        
        data.sampledImages.push_back(resource);
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
        resource.members = ExtractStructMembers(compiler, type);

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