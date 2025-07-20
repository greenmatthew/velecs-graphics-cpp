/// @file    ShaderReflectionData.cpp
/// @author  Matthew Green
/// @date    2025-07-14 16:40:06
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/Reflection/ShaderReflectionData.hpp"

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

ShaderReflectionData ShaderReflectionData::Merge(const ShaderReflectionData& other) const
{
    ShaderReflectionData merged;
    
    merged.uniformBuffers = MergeResourceVector(uniformBuffers, other.uniformBuffers);
    merged.textures = MergeResourceVector(textures, other.textures);
    merged.pushConstants = MergeResourceVector(pushConstants, other.pushConstants);
    
    return merged;
}

std::ostream& operator<<(std::ostream& os, const ShaderReflectionData& data) {
    os << "ShaderReflectionData {\n";
    
    // Push Constants section
    if (!data.pushConstants.empty()) {
        os << "  Push Constants (" << data.pushConstants.size() << "):\n";
        for (const auto& resource : data.pushConstants) {
            os << "    " << resource << "\n";
        }
        os << "\n";
    }
    
    // Uniform Buffers section
    if (!data.uniformBuffers.empty()) {
        os << "  Uniform Buffers (" << data.uniformBuffers.size() << "):\n";
        for (const auto& resource : data.uniformBuffers) {
            os << "    " << resource << "\n";
        }
        os << "\n";
    }
    
    // Textures section
    if (!data.textures.empty()) {
        os << "  Textures (" << data.textures.size() << "):\n";
        for (const auto& resource : data.textures) {
            os << "    " << resource << "\n";
        }
        os << "\n";
    }
    
    // Summary
    os << "  Total Resources: " << (data.pushConstants.size() + data.uniformBuffers.size() + data.textures.size()) << "\n";
    os << "}";
    
    return os;
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

std::vector<ShaderResource> ShaderReflectionData::MergeResourceVector(
    const std::vector<ShaderResource>& a, 
    const std::vector<ShaderResource>& b
)
{
    std::vector<ShaderResource> merged = a; // Start with all resources from 'a'
    
    // Process each resource from 'b'
    for (const auto& resourceB : b)
    {
        // Try to find matching resource in our merged list
        auto it = std::find_if(merged.begin(), merged.end(), 
            [&resourceB](const ShaderResource& existing) {
                return existing == resourceB;
            });
        
        if (it != merged.end())
        {
            // Found duplicate - merge stages (now we can modify 'merged')
            it->stages |= resourceB.stages;
        }
        else
        {
            // New resource - add it
            merged.push_back(resourceB);
        }
    }
    
    return merged;
}

} // namespace velecs::graphics
