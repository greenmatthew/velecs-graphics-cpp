/// @file    ShaderReflectionData.hpp
/// @author  Matthew Green
/// @date    2025-07-14 12:35:05
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Shader/Reflection/ShaderResource.hpp"

#include <vector>

namespace velecs::graphics {

/// @struct ShaderReflectionData
/// @brief Brief description.
///
/// Rest of description.
struct ShaderReflectionData {
public:
    // Enums

    // Public Fields

    std::vector<ShaderResource> uniformBuffers;
    std::vector<ShaderResource> textures;
    std::vector<ShaderResource> pushConstants;

    // Constructors and Destructors

    /// @brief Default constructor.
    ShaderReflectionData() = default;

    /// @brief Default deconstructor.
    ~ShaderReflectionData() = default;

    // Public Methods

    inline bool HasPushConstants() const { return pushConstants.size() > 0; }

    inline bool HasTextures() const { return textures.size() > 0; }

    // GetVkStructureForPushConstants
    // GetVkStructureForUniformBuffer
    // GetVkStructureForTextureSamples

    ShaderReflectionData Merge(const ShaderReflectionData& other) const;

    friend std::ostream& operator<<(std::ostream& os, const ShaderReflectionData& data);

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods

    static std::vector<ShaderResource> MergeResourceVector(const std::vector<ShaderResource>& a, const std::vector<ShaderResource>& b);
};

} // namespace velecs::graphics
