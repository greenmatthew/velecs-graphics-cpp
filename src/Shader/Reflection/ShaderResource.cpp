/// @file    ShaderResource.cpp
/// @author  Matthew Green
/// @date    2025-07-14 16:36:32
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/Reflection/ShaderResource.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

bool ShaderResource::operator==(const ShaderResource& other) const
{
    if (type != other.type) return false;
    
    if (type == ShaderResourceType::PushConstant)
    {
        // Push constants: use name + size
        return name == other.name && size == other.size;
    }
    
    // For descriptor resources: binding location is the unique ID
    return set == other.set && binding == other.binding;
}

bool ShaderResource::operator!=(const ShaderResource& other) const
{
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const ShaderResource& resource)
{
    os << "ShaderResource {\n";
    os << "  name: " << resource.name << "\n";
    os << "  type: ";
    
    // Convert enum to string
    switch (resource.type) {
        case ShaderResourceType::Unknown: os << "Unknown"; break;
        case ShaderResourceType::PushConstant: os << "PushConstant"; break;
        case ShaderResourceType::UniformBuffer: os << "UniformBuffer"; break;
        case ShaderResourceType::StorageImage: os << "StorageImage"; break;
        case ShaderResourceType::SampledImage: os << "SampledImage"; break;
        default: os << "Unknown(" << static_cast<int>(resource.type) << ")"; break;
    }
    os << "\n";
    
    os << "  stages: 0x" << std::hex << resource.stages << std::dec;
    
    // Decode stage flags for readability
    os << " (";
    bool first = true;
    if (resource.stages & VK_SHADER_STAGE_VERTEX_BIT) {
        if (!first) os << " | ";
        os << "VERTEX";
        first = false;
    }
    if (resource.stages & VK_SHADER_STAGE_FRAGMENT_BIT) {
        if (!first) os << " | ";
        os << "FRAGMENT";
        first = false;
    }
    if (resource.stages & VK_SHADER_STAGE_GEOMETRY_BIT) {
        if (!first) os << " | ";
        os << "GEOMETRY";
        first = false;
    }
    // Add more stages as needed
    os << ")\n";
    
    os << "  offset: " << resource.offset << "\n";
    os << "  size: " << resource.size << "\n";
    os << "  set: " << resource.set << "\n";
    os << "  binding: " << resource.binding << "\n";

    if (resource.members.size() > 0)
    {
        os << "  members: {\n";
        for (const auto& member : resource.members)
        {
            std::cout << "    " << member << std::endl;
        }
        os << "  }\n";
    }

    os << "}";
    
    return os;
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::graphics
