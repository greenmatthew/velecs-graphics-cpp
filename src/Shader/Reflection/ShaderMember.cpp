/// @file    ShaderMember.cpp
/// @author  Matthew Green
/// @date    2025-08-02 12:49:11
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/graphics/Shader/Reflection/ShaderMember.hpp"

#include <sstream>

namespace velecs::graphics {

// Public Fields

// Constructors and Destructors

// Public Methods

std::ostream& operator<<(std::ostream& os, const ShaderMember& member)
{
    os << "ShaderMember { name: \"" << member.name << "\", type: ";
    
    // Convert enum to string
    switch (member.type) {
        case ShaderMemberType::Unknown: os << "Unknown"; break;
        case ShaderMemberType::Bool: os << "Bool"; break;
        case ShaderMemberType::Float: os << "Float"; break;
        case ShaderMemberType::Int: os << "Int"; break;
        case ShaderMemberType::UInt: os << "UInt"; break;
        case ShaderMemberType::Vec2: os << "Vec2"; break;
        case ShaderMemberType::Vec3: os << "Vec3"; break;
        case ShaderMemberType::Vec4: os << "Vec4"; break;
        case ShaderMemberType::Mat4: os << "Mat4"; break;
        case ShaderMemberType::Struct: os << "Struct"; break;
        case ShaderMemberType::Array: os << "Array"; break;
        default: os << "Unknown(" << static_cast<int>(member.type) << ")"; break;
    }
    
    // Show array notation if arraySize > 1
    if (member.arraySize > 0) {
        os << "[" << member.arraySize << "]";
    }
    
    os << " (offset: " << member.offset << ", size: " << member.size << ")";
    
    // Show nested struct members on separate lines if this is a struct
    if (member.type == ShaderMemberType::Struct && !member.members.empty()) {
        os << " {\n";
        for (const auto& nestedMember : member.members) {
            os << "    " << nestedMember << "\n";
        }
        os << "  }";
    } else {
        os << " }";
    }
    
    return os;
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::graphics
