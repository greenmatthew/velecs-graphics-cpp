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

#include <vector>

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

    /// @brief Default constructor.
    PushConstant() = default;

    /// @brief Default deconstructor.
    ~PushConstant() = default;

    // Public Methods

    inline bool HasData() const { return !_data.empty(); }

    inline const uint32_t GetSize() const { return static_cast<uint32_t>(_data.size()); }

    inline const uint8_t* GetData() const { return _data.data(); }

    template<typename T>
    void Set(const T& data, const ShaderReflectionData& reflectionData)
    {
        if (reflectionData.pushConstants.size() > 1)
        {
            throw std::runtime_error("Multiple push constants not yet supported.");
        }
        
        if (reflectionData.pushConstants.empty())
        {
            throw std::runtime_error("Shader does not define any push constants.");
        }

        const auto& pushConstResource = reflectionData.pushConstants[0];
        
        if (sizeof(T) != pushConstResource.size)
        {
            throw std::invalid_argument("Push constant size mismatch");
        }

        _data.resize(sizeof(T));
        std::memcpy(_data.data(), &data, sizeof(T));
    }

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    std::vector<uint8_t> _data;

    // Private Methods
};

} // namespace velecs::graphics
