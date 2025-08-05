/// @file    PushConstant.hpp
/// @author  Matthew Green
/// @date    2025-08-03 13:54:04
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Shader/Reflection/ShaderReflectionData.hpp"

#include <vulkan/vulkan_core.h>

#include <vector>
#include <optional>
#include <typeinfo>
#include <type_traits>

namespace velecs::graphics {

/// @class PushConstant
/// @brief Brief description.
///
/// Rest of description.
class PushConstant {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Creates a configured PushConstant for a specific type
    /// @tparam T The push constant struct type
    /// @param stageFlags Which shader stages will use this push constant
    /// @param reflectionData Shader reflection data for validation
    /// @param offset Optional offset in bytes (default: 0)
    /// @return Configured PushConstant instance
    template<typename T>
    static PushConstant Create(VkShaderStageFlags stageFlags, const ShaderReflectionData& reflectionData, uint32_t offset = 0)
    {
        static_assert(std::is_standard_layout_v<T>, "Push constant type must have standard layout");
        
        ValidateWithReflection<T>(reflectionData);

        // Type info for runtime validation
        const size_t typeHash = typeid(T).hash_code();
        const size_t typeSize = sizeof(T);

        // Create the Vulkan range
        VkPushConstantRange range{};
        range.stageFlags = stageFlags;
        range.offset = offset;
        range.size = static_cast<uint32_t>(typeSize);
        
        return PushConstant{typeHash, typeSize, range};
    }

    /// @brief Default deconstructor.
    ~PushConstant() = default;

    // Public Methods

    /// @brief Updates the push constant data (fast runtime operation)
    /// @tparam T The push constant struct type (must match Configure() call)
    /// @param data The new push constant data
    template<typename T>
    void UpdateData(const T& data)
    {
        static_assert(std::is_standard_layout_v<T>, "Push constant type must have standard layout");
        
        // Validate type matches configuration
        if (_typeHash != typeid(T).hash_code() || _typeSize != sizeof(T))
            throw std::runtime_error("Push constant type mismatch. Type must match Configure() call");
        
        // Fast data copy - no validation needed
        std::memcpy(_data.data(), &data, sizeof(T));
    }

    /// @brief Checks if push constant has data set
    inline bool HasData() const { return !_data.empty(); }

    /// @brief Gets the size of the push constant data
    inline uint32_t GetSize() const { return static_cast<uint32_t>(_data.size()); }

    /// @brief Gets a pointer to the push constant data
    inline const uint8_t* GetData() const { return _data.data(); }

    /// @brief Gets the Vulkan push constant range
    /// @return The VkPushConstantRange, or nullopt if not configured
    inline const VkPushConstantRange& GetRange() const { return _range; }

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    size_t _typeHash;
    size_t _typeSize;
    VkPushConstantRange _range;

    std::vector<uint8_t> _data;

    // Private Methods

    /// @brief Default constructor.
    PushConstant(const size_t _typeHash, const size_t _typeSize, const VkPushConstantRange range);

    /// @brief Validates the type against shader reflection data
    template<typename T>
    static void ValidateWithReflection(const ShaderReflectionData& reflectionData)
    {
        if (reflectionData.pushConstants.size() > 1)
            throw std::runtime_error("Multiple push constants not yet supported");
        
        if (reflectionData.pushConstants.empty())
            throw std::runtime_error("Shader does not define any push constants");

        const auto& pushConstResource = reflectionData.pushConstants[0];
        
        if (sizeof(T) != pushConstResource.size)
            throw std::invalid_argument("Push constant size mismatch between C++ struct and shader definition");
    }
};

} // namespace velecs::graphics
