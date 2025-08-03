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
    merged.sampledImages = MergeResourceVector(sampledImages, other.sampledImages);
    merged.pushConstants = MergeResourceVector(pushConstants, other.pushConstants);
    
    return merged;
}

std::ostream& operator<<(std::ostream& os, const ShaderReflectionData& data) {
    os << "ShaderReflectionData {\n";
    
    // Push Constants section
    if (!data.pushConstants.empty())
    {
        os << "Push Constants: [\n\n";
        for (const auto& resource : data.pushConstants)
        {
            os << resource << "\n";
        }
        os << "]\n\n";
    }
    
    // Uniform Buffers section
    if (!data.uniformBuffers.empty())
    {
        os << "Uniform Buffers: [\n\n";
        for (const auto& resource : data.uniformBuffers)
        {
            os << resource << "\n";
        }
        os << "]\n\n";
    }

    // Storage images section
    if (!data.storageImages.empty()) {
        os << "Storage Images: [\n\n";
        for (const auto& resource : data.storageImages)
        {
            os << resource << "\n";
        }
        os << "]\n\n";
    }
    
    // Sampled images section
    if (!data.sampledImages.empty()) {
        os << "Sampled Images: [\n\n";
        for (const auto& resource : data.sampledImages)
        {
            os << resource << (data.sampledImages.size() > 1 ? "," : "") << "\n";
        }
        os << "]\n\n";
    }
    
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
