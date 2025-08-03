/// @file    ShaderResource.hpp
/// @author  Matthew Green
/// @date    2025-07-14 15:00:19
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Shader/Reflection/ShaderResourceType.hpp"
#include "velecs/graphics/Shader/Reflection/ShaderMember.hpp"

#include <vulkan/vulkan_core.h>

#include <spirv_cross/spirv_cross.hpp>

#include <iostream>

namespace velecs::graphics {

/// @struct ShaderResource
/// @brief Brief description.
///
/// Rest of description.
struct ShaderResource {
public:
    // Enums

    // Public Fields

    ShaderResourceType type;
    VkShaderStageFlags stages;

    std::string name;
    uint32_t offset{0};
    uint32_t size{0};
    uint32_t set{0};
    uint32_t binding{0};

    std::vector<ShaderMember> members;

    // Constructors and Destructors

    /// @brief Default constructor.
    ShaderResource() = default;

    /// @brief Default deconstructor.
    ~ShaderResource() = default;

    // Public Methods

    bool operator==(const ShaderResource& other) const;
    bool operator!=(const ShaderResource& other) const;

    friend std::ostream& operator<<(std::ostream& os, const ShaderResource& resource);

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
