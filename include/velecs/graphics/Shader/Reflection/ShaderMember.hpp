/// @file    ShaderMember.hpp
/// @author  Matthew Green
/// @date    2025-08-02 12:19:06
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Shader/Reflection/ShaderMemberType.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace velecs::graphics {

/// @struct ShaderMember
/// @brief Brief description.
///
/// Rest of description.
struct ShaderMember {
public:
    // Enums

    // Public Fields

    std::string name;
    uint32_t offset;
    uint32_t size;
    uint32_t arraySize;
    ShaderMemberType type;

    // For nested structs
    std::vector<ShaderMember> members;  // Recursive for nested structs

    // Constructors and Destructors

    /// @brief Default constructor.
    ShaderMember() = default;

    /// @brief Default deconstructor.
    ~ShaderMember() = default;

    // Public Methods

    friend std::ostream& operator<<(std::ostream& os, const ShaderMember& member);

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
