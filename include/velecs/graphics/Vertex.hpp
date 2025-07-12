/// @file    Vertex.hpp
/// @author  Matthew Green
/// @date    2025-07-12 17:58:35
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/graphics/Color32.hpp"
#include "velecs/graphics/VertexBufferParamsBuilder.hpp"

#include <velecs/math/Vec3.hpp>

namespace velecs::graphics {

/// @struct Vertex
/// @brief Brief description.
///
/// Rest of description.
struct Vertex {
    using Vec3 = velecs::math::Vec3;

    Vec3 pos;
    uint16_t normX, normY;
    Color32 color;

    inline static VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() {
        static const auto createInfo = VertexBufferParamsBuilder()
            .AddBinding(sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX, [](auto& binding) {
                binding.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos))
                       .AddAttribute(VK_FORMAT_R16G16_SNORM, offsetof(Vertex, normX))
                       .AddAttribute(VK_FORMAT_B8G8R8A8_SRGB, offsetof(Vertex, color));
            })
            .GetCreateInfo();
        return createInfo;
    }
};

} // namespace velecs::graphics
