/// @file    ShaderResourceType.hpp
/// @author  Matthew Green
/// @date    2025-07-14 15:10:09
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

namespace velecs::graphics {

/// @enum ShaderResourceType
/// @brief Brief description.
///
/// Rest of description.
enum class ShaderResourceType {
    Unknown = 0,
    PushConstant,
    UniformBuffer,
    StorageImage,
    SampledImage,
};

} // namespace velecs::graphics
