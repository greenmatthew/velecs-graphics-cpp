/// @file    ShaderMemberType.hpp
/// @author  Matthew Green
/// @date    2025-08-02 12:19:29
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

namespace velecs::graphics {

/// @enum ShaderMemberType
/// @brief Enumeration of supported shader variable types for reflection.
///
/// This enum represents the different data types that can be found within shader
/// resources (uniform buffers, push constants, etc.) during SPIR-V reflection.
/// Only commonly used types are currently supported - unsupported types will be
/// marked as Unknown and can be added as needed.
///
/// The types correspond to GLSL/HLSL shader variable types and can be mapped to
/// equivalent C++ types for material parameter setting.
enum class ShaderMemberType {
    /// @brief Unknown or unsupported type
    /// Used for shader types that are not yet supported by the reflection system.
    /// Parameters with this type will throw errors if accessed through Material API.
    Unknown,

    // Primitives
    
    /// @brief Boolean type (GLSL: bool, HLSL: bool)
    /// Maps to C++ bool
    Bool,
    
    /// @brief 32-bit floating point (GLSL: float, HLSL: float)
    /// Maps to C++ float
    Float,
    
    /// @brief 32-bit signed integer (GLSL: int, HLSL: int)
    /// Maps to C++ int32_t
    Int,
    
    /// @brief 32-bit unsigned integer (GLSL: uint, HLSL: uint)
    /// Maps to C++ uint32_t
    UInt,

    // Vectors
    
    /// @brief 2-component floating point vector (GLSL: vec2, HLSL: float2)
    /// Maps to C++ Vec2 or glm::vec2
    Vec2,
    
    /// @brief 3-component floating point vector (GLSL: vec3, HLSL: float3)
    /// Maps to C++ Vec3 or glm::vec3
    Vec3,
    
    /// @brief 4-component floating point vector (GLSL: vec4, HLSL: float4)
    /// Maps to C++ Vec4 or glm::vec4
    Vec4,

    // Matrices
    
    /// @brief 4x4 floating point matrix (GLSL: mat4, HLSL: float4x4)
    /// Maps to C++ Mat4 or glm::mat4
    Mat4,

    // Complex Types
    
    /// @brief User-defined struct type
    /// Contains nested members accessible via the ShaderMember::members field.
    /// Struct members can be recursively accessed using dot notation (e.g., "lighting.ambient").
    Struct,
    
    /// @brief Array of any supported type
    /// The base element type and array size are determined by other ShaderMember fields.
    /// Individual elements can be accessed using bracket notation (e.g., "lights[0]").
    Array,
};

} // namespace velecs::graphics
