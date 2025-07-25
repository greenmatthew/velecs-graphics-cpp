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
using velecs::math::Vec3;

namespace velecs::graphics {

/// @struct Vertex
/// @brief Brief description.
///
/// Rest of description.
struct Vertex {
    Vec3 pos{Vec3::ZERO};            // location 0
    Color32 color{Color32::MAGENTA}; // location 1

    static_assert(sizeof(Vec3) == 12);
    static_assert(sizeof(Color32) == 4);

    static const VkPipelineVertexInputStateCreateInfo& GetVertexInputInfo() {
        static const auto builder = VertexBufferParamsBuilder()
            .AddBinding(sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX, [](auto& binding) {
                binding.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos))
                       .AddAttribute(VK_FORMAT_R8G8B8A8_UNORM, offsetof(Vertex, color))
                       ;
            });
        
        return builder.GetCreateInfo(); // Now returns const reference to persistent data
    }
};

} // namespace velecs::graphics
